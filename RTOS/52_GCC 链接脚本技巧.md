# GCC 链接脚本详解

## 简介

本文详细详细分析 GCC 链接脚本。

## MEMORY

MEMORY 是 GCC 连接器支持语法中的内存块配置命令，一个链接脚本最多一个 MEMORY 命令，MEMORY 的语法如下：

```HTTP
MEMORY
{
NAME [(ATTR)] : ORIGIN = ORIGIN, LENGTH = LEN
...
}
```

各个配置项的说明如下：

| 配置项 | 说明                                                         |
| ------ | ------------------------------------------------------------ |
| NAME   | 内存区域的名字，可根据硬件的实际内存分布来取一个对应的名字，例如 FLASH、SRAM 或者 ATCM 都行。 |
| ATTR   | 属性列表，它指出是否为一个没有在连接脚本中进行显式映射地输入段使用一个特定的内存区域。ATTR字符串必须包含下面字符中的一个，常用的有 R(只读)、W（可读写）、X（可执行）、A（可分配）、I（已初始化）。 |
| ORIGIN | 内存区域地始地址 (可以是表达式)。                            |
| LEN    | 内存区域长度（以字节为单位）的表达式。                       |

现在我们来看一看示例的 MEMORY 命令是怎么写的：

```C
 OUTPUT_FORMAT("elf32-littlearm")  /* 设置输出格式为 32 位可执行程序 */
_region_min_align = 64;
MEMORY
{
    /* 起始地址为 0x100000，这一段对应真实物理内存 BTCM，只是名字沿用了默认名称 FLASH */
    FLASH (rx) : ORIGIN = 0x100000, LENGTH = 256 * 1K

    /* 起始地址为 0x200000，这一段对应真实物理内存 XXX_REGION，只是名字沿用了默认名称 SRAM */   
    SRAM (wx) : ORIGIN = 0x200000, LENGTH = 256 * 1K
}
```

由 MEMORY 可以知道，整个系统中总共有 5 段地址（输出段）用于存储数据和代码，那么接下来的问题就是需要将哪些数据或者代码（输入段，其实就是各种 .o 文件中的符号）分别加载到这五段地址空间。

## 输出段

下面是输出段的标准格式，[>region] 指定输出段分布在内存上的地址，也就是说将该 section 分配到先前定义好的内存区域里，也就是在 MEMORY 区域定义的段，如 FLASH 、SRAM 以及 XXX_REGION 等。

```C
section [address] [(type)] : [AT(lma)]
  {
    output-section-command
    output-section-command
    ...
  } [>region] [AT>lma_region] [:phdr :phdr ...] [=fillexp]
```

接下来看一下 Zephyr 实际的链接脚本，我选取一些重要的节，解释每一行体现的细节：

### FLASH 段

rom_start section 主要是放置了系统的中断向量表：

```C
    __rom_region_start = 0x100000;   /* 指定当前地址为 0x100000 */
    rom_start :                      /* 定义一个 section 为 rom_start */
    {
        . = 0x0;
        . = ALIGN(4);                 
        _vector_start = .;           /* 将向量表指定到以 0x100000 起始的地址  */
        KEEP(*(.exc_vector_table))
        KEEP(*(".exc_vector_table.*"))
        KEEP(*(.gnu.linkonce.irq_vector_table*))
        KEEP(*(.vectors))
        _vector_end = .;
    } > FLASH                        /* 将该 section 放置在 FLASH 段中*/
```

text section 主要存放了要执行的代码：

```C
    text :                          /* 定义一个 section 为 text */
    {
        __text_region_start = .;
        *(.text)
        *(".text.*")
        *(.gnu.linkonce.t.*)
        *(.glue_7t) *(.glue_7) *(.vfp11_veneer) *(.v4_bx)
    } > FLASH                       /* 将该 section 放置在 FLASH 段中*/
    __text_region_end = .;
```

其他需要放置在 FLASH 段的 section 不再一一列举，还有 `.ARM.exidx`、`initlevel`（用于组件分级初始化）、devices（用于设备分级初始化）、`sw_isr_table`（`gic` 软件中断表）、`log_strings_sections`、`rodata` 等等。

### SRAM 段

接下来要将一些程序运行过程中所需要的数据放置待 SRAM 段中，BSS 段用于存放一些初始化为 0 的数据，在生成的 bin 文件中不占用空间，但是在运行时需要在指定的内存地址留出空间。

```C
    . = 0x200000;                          /* 指定当前地址为 0x100000 */
    . = ALIGN(_region_min_align);
    _image_ram_start = .;
    bss (NOLOAD) :                         /* 定义一个 section 为 bss */
    {
        . = ALIGN(4);
        __bss_start = .;
        __kernel_ram_start = .;
        *(.bss)
        *(".bss.*")
        *(COMMON)
        *(".kernel_bss.*")
        __bss_end = ALIGN(4);
    } > SRAM                               /* 将该 section 放置在 SRAM 段中 */
```

`noinit section`：

```C
noinit (NOLOAD) :
{
        *(.noinit)
        *(".noinit.*")
} > SRAM                                   /* 将该 section 放置在 SRAM 段中 */
```

`datas` 用于存放初始化过的全局变量：

```C
    datas : 
    {
        __data_region_start = .;
        __data_start = .;
        *(.data)
        *(".data.*")
        *(".kernel.*")
        __data_end = .;
    } > SRAM                                /* 将该 section 放置在 SRAM 段中 */ 
```

还有一些其他数据节例如：`initshell`、`log_dynamic_sections`、`k_timer_area` 等一系列内核对象的数据。

### 添加输出段

通过上面的分析可知，如果想要添加新的输出段，在 MEMORY 里添加新的分配区域即可，如果是添加 `xMEM`，那就是在 MEMORY 里添加如下代码：

```C
xMEM (wx) : ORIGIN = 0x200000, LENGTH = 512 * 1K
```

然后再将想要放置在这个段内的 section 分配到 `xMEM` 段中。

### 重定位 library

可以将指定 `.o` 文件或者 `lib` 文件中的不同段，text、rodata、data、bss 段分别指定到不同自定义 section 中。

```c
    . = ORIGIN(XXX_REGION);
    _XXX_TEXT_SECTION_NAME :
    {
        ../app/libxxx.a(.text*)  // 重定位 lib 文件中的 text 段
        xxx.o (.text*)           // 重定位 obj 文件中的 text 段

        . = ALIGN(_region_min_align);
        __XXX_text_end = .;
    } > XXX_REGION
    __XXX_text_start = ADDR(_XXX_TEXT_SECTION_NAME);
    __XXX_text_size = SIZEOF(_XXX_TEXT_SECTION_NAME);

    _XXX_RODATA_SECTION_NAME :
    {
        ../app/libapp.a(.rodata*)

        . = ALIGN(_region_min_align);
        __XXX_rodata_end = .;
    }> XXX_REGION

    __XXX_rodata_size = __XXX_rodata_end - __XXX_rodata_start;
    __XXX_rodata_start = ADDR(_XXX_RODATA_SECTION_NAME);

    _XXX_DATA_SECTION_NAME :
    {
        ../app/libapp.a(.data*)

        . = ALIGN(_region_min_align);
        __XXX_data_end = .;
    }> XXX_REGION

    __XXX_data_start = ADDR(_XXX_DATA_SECTION_NAME);
    __XXX_data_size = SIZEOF(_XXX_DATA_SECTION_NAME);

    _XXX_BSS_SECTION_NAME :
    {
        ../app/libapp.a(.bss*)

        . = ALIGN(_region_min_align);
        __XXX_bss_end = .;
    } > XXX_REGION

    __XXX_bss_start = ADDR(_XXX_BSS_SECTION_NAME);
    __XXX_bss_size = SIZEOF(_XXX_BSS_SECTION_NAME);
```

## 小结

通过修改链接脚本可以让我们灵活地部署应用程序，将数据存放在指定地址空间。也可以帮助我们将程序的不同数据段生成不同的 bin 文件，用于灵活地程序加载。



