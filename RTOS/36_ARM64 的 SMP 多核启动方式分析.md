# ARM64 的 SMP 多核启动流程分析

工作中遇到的多核 ARM CPU 越来越多，总结分享一些多核启动的知识，希望能帮助更多小伙伴。

在 ARM64 架构下如果想要启动多核，有 spin-table 和 psci 两种方式，下面针对这两种启动流程进行分析。

## 代码版本

- boot-wrapper-aarch64 version : 28932c41e14d730b8b9a7310071384178611fb32

- linux v5.14

## 多核 CPU 的启动方式

嵌入式系统的启动的基本流程是先运行 `bootloader` ，然后由 `bootloader` 引导启动 kernel，这里无论启动的是 rt-thread 或者是 linux 原理都是一样的。

上电后所有的 `CPU` 都会从 `bootrom` 里面开始执行代码，为了防止并发造成的一些问题，需要将除了 `primary cpu` 以外的 `cpu` 拦截下来，这样才能保证启动的顺序是可控的。

## spin-table 启动方法

在启动的过程中，`bootloader` 中有一道栅栏，它拦住了除了 `cpu0` 外的其他 `cpu`。`cpu0` 直接往下运行，进行设备初始化以及运行 `Kernel`。其他 `cpu0` 则在栅栏外进入睡眠状态。

`cpu0` 在初始化 `smp` 的时候，会在 `cpu-release-addr` 里面填入一个地址并唤醒其他 `cpu`。这时睡眠的 `cpu` 接受到信号，醒来的时候会先检查 `cpu-release-addr` 这个地址里面的数据是不是有效。如果该地址是有效的（非 0 ），意味着自己需要真正开始启动了，接下来他会跳转到。

下面我们看看 `arm64` 里面的实现，在 `arch/arm64/boot/dts/xxx.dts` 中有如下描述：

```c
cpu@0 {
    device_type = "cpu";
    compatible = "arm,armv8";
    reg = <0x0 0x0="">;
    enable-method = "spin-table"; /* 选择使用 spin-table 方式启动  */
    cpu-release-addr = <0x0 0x8000fff8="">;
};
```

在 `arch/arm64/kernel/smp_spin_table.c` 中处理了向其他 cpu 发送信号的方法：
1. 先是获取 release_addr 的虚拟地址
2. 向该地址写入从 cpu 的入口地址
3. 通过 sev() 指令唤醒其他 cpu

```c
static int smp_spin_table_cpu_prepare(unsigned int cpu)
{
	__le64 __iomem *release_addr;
	phys_addr_t pa_holding_pen = __pa_symbol(function_nocfi(secondary_holding_pen));

	if (!cpu_release_addr[cpu])
		return -ENODEV;

	/*
	 * The cpu-release-addr may or may not be inside the linear mapping.
	 * As ioremap_cache will either give us a new mapping or reuse the
	 * existing linear mapping, we can use it to cover both cases. In
	 * either case the memory will be MT_NORMAL.
	 */
	release_addr = ioremap_cache(cpu_release_addr[cpu],
				     sizeof(*release_addr));
	if (!release_addr)
		return -ENOMEM;

	/*
	 * We write the release address as LE regardless of the native
	 * endianness of the kernel. Therefore, any boot-loaders that
	 * read this address need to convert this address to the
	 * boot-loader's endianness before jumping. This is mandated by
	 * the boot protocol.
	 */
	writeq_relaxed(pa_holding_pen, release_addr);
	dcache_clean_inval_poc((__force unsigned long)release_addr,
			    (__force unsigned long)release_addr +
				    sizeof(*release_addr));

	/*
	 * Send an event to wake up the secondary CPU.
	 */
	sev();

	iounmap(release_addr);

	return 0;
}
```

`Bootloader` 部分以 `boot-wrapper-aarch64` 中的代码做示例，非主 CPU 会轮询检查 mbox（其地址等同cpu-release-addr）中的值，当其值为 0 的时候继续睡眠，否则就跳转到内核执行，代码如下所示：


```c
/**
 * Wait for an address to appear in mbox, and jump to it.
 *
 * @mbox: location to watch
 * @invalid: value of an invalid address, 0 or -1 depending on the boot method
 * @is_entry: when true, pass boot parameters to the kernel, instead of 0
 */
void __noreturn spin(unsigned long *mbox, unsigned long invalid, int is_entry)
{
	unsigned long addr = invalid;

	while (addr == invalid) {
		wfe();
		addr = *mbox;
	}

	if (is_entry)
#ifdef KERNEL_32
		jump_kernel(addr, 0, ~0, (unsigned long)&dtb, 0);
#else
		jump_kernel(addr, (unsigned long)&dtb, 0, 0, 0);
#endif

	jump_kernel(addr, 0, 0, 0, 0);

	unreachable();
}

/**
 * Primary CPU finishes platform initialisation and jumps to the kernel.
 * Secondaries are parked, waiting for their mbox to contain a valid address.
 *
 * @cpu: logical CPU number
 * @mbox: location to watch
 * @invalid: value of an invalid address, 0 or -1 depending on the boot method
 */
void __noreturn first_spin(unsigned int cpu, unsigned long *mbox,
			   unsigned long invalid)
{
	if (cpu == 0) {
		init_platform();

		*mbox = (unsigned long)&entrypoint;
		sevl();
		spin(mbox, invalid, 1);
	} else {
		*mbox = invalid;
		spin(mbox, invalid, 0);
	}

	unreachable();
}

```

## PSCI 启动方法

另外一种 enable-method 就是 PSCI，依旧先从 kernel 开始分析。先看 `arch/arm64/boot/dts/mediatek/mt8173.dtsi` 文件，里面 `cpu` 节点选择了PSCI 的方法：

```c
cpu0: cpu@0 {
    compatible = "arm,cortex-a53";
    device_type = "cpu";
    enable-method = "psci";    /* 启动方式选择 PSCI */
    operating-points-v2 = <&cpu_opp_table>;
    reg = <0x0>;
    cpu-idle-states = <&CPU_SLEEP_0>;
};
```

并且有一个 `PSCI` 的节点：

```c
psci {
    compatible = "arm,psci-1.0", "arm,psci-0.2", "arm,psci";
    method = "smc";
    cpu_suspend   = <0x84000001>;
    cpu_off	      = <0x84000002>;
    cpu_on	      = <0x84000003>;
};
```

在 `PSCI` 中的节点详细说明请参考文档：kernel/Documentation/devicetree/bindings/arm/psci.txt。 在此仅说一下 method 字段。该字段有两个可选值：smc 和 hvc。表示调用 PSCI 功能使用什么指令。smc、hvc、svc 这些指令都是由低运行级别向更高级别请求服务的指令。

和系统调用一样。调用了该指令，cpu 会进入异常切入更高的权限。异常处理程序根据下面传上来的参数决定给予什么服务，smc 陷入 EL3，hvc 陷入 EL2，svc 陷入EL1。在 ARMv8 里面，EL3 总是是 secure 状态，EL2 是虚拟机管态，EL1 是普通的系统态。

接下来可以看看 `arch/arm64/kernel/psci.c` 里面的代码，psci_ops.cpu_on 最终调用 smc  call：

```c
static int cpu_psci_cpu_boot(unsigned int cpu)
{
	phys_addr_t pa_secondary_entry = __pa_symbol(function_nocfi(secondary_entry));
	int err = psci_ops.cpu_on(cpu_logical_map(cpu), pa_secondary_entry);
	if (err)
		pr_err("failed to boot CPU%d (%d)\n", cpu, err);

	return err;
}

static unsigned long __invoke_psci_fn_smc(unsigned long function_id,
			unsigned long arg0, unsigned long arg1,
			unsigned long arg2)
{
	struct arm_smccc_res res;

	arm_smccc_smc(function_id, arg0, arg1, arg2, 0, 0, 0, 0, &res);
	return res.a0;
}

```

Bootloader 以 `boot-wrapper-aarch64` 作分析，看 psci.c 里的 psci_call 实现函数，通过 fid 与 PSCI_CPU_OFF 和 PSCI_CPU_ON 相比，找出需要执行的动作：

```c
long psci_call(unsigned long fid, unsigned long arg1, unsigned long arg2)
{
	switch (fid) {
	case PSCI_CPU_OFF:
		return psci_cpu_off();

	case PSCI_CPU_ON_64:
		return psci_cpu_on(arg1, arg2);

	default:
		return PSCI_RET_NOT_SUPPORTED;
	}
}
```

当然 `boot-wrapper-aarch64` 里也需要同样的定义：

```c
#define PSCI_CPU_OFF        0x84000002
#define PSCI_CPU_ON_32      0x84000003
#define PSCI_CPU_ON_64      0xc4000003
```

`boot-wrapper-aarch64` 按照和 `kernel` 约定的好参数列表，为目标 `cpu` 设置好跳转地址，然后返回到 `kernel ` 执行，下面给出关键代码说明：

```c
static int psci_cpu_on(unsigned long target_mpidr, unsigned long address)
{
	int ret;
	unsigned int cpu = find_logical_id(target_mpidr);
	unsigned int this_cpu = this_cpu_logical_id();

	if (cpu == MPIDR_INVALID)
		return PSCI_RET_INVALID_PARAMETERS;

	bakery_lock(branch_table_lock, this_cpu);
	ret = psci_store_address(cpu, address);   /* 写入启动地址  */
	bakery_unlock(branch_table_lock, this_cpu);

	return ret;
}
```

## 总结

目前比较主流的多核启动方式是 PSCI，一般正式的产品都有 ATF，通过 PSCI 可以实现 CPU 的开启关闭以及挂起等操作。在实际的移植工作过程中，如果有带有 ATF 的 bootloader 那多核移植就相对容易很多，如果没有的话，也可以采用 spin_table 的方式来启动多核。