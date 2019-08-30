# 使用 STM32 通用 Bootloader ，让 OTA 更加 Easy

最新 RT-Thread 发布了一款 STM32 系列的通用 BootLoader，有兴趣的小伙伴可以用用，我试了下，挺方便的。

RT-Thread 通用 Bootloader 有如下特点：

- 以 bin 文件的形式提供，无需修改即可使用
- 资源占用小，ROM 最小只需要 16KB，最大 32KB
- 适用于多系列 STM32 芯片（目前支持 F1 F4 和 L4 系列 ）
- 支持各种 SPI Flash 存储固件
- 支持固件加解密功能
- 支持多种固件压缩方式
- 支持恢复出厂固件功能
- 以上功能均可自由配置

发布文章看这里： 

[使用 STM32 通用 Bootloader ，让 OTA 更加 Easy](https://mp.weixin.qq.com/s?__biz=MzIwMzA2NzI1Ng==&mid=2655157340&idx=1&sn=cc484445e5159364e00c9b732d5f51b3&chksm=8d63c64bba144f5df5ff0c55b1c92de4398eee915846fcdd3c944b543b81da8b12ca1674bcf0&mpshare=1&scene=1&srcid=0329pskukobmDRuzGMR1Wczj#rd)

官方文档中心看这里：

[STM32 通用 BootLoader 应用笔记](https://www.rt-thread.org/document/site/application-note/system/rtboot/an0028-rtboot/)

下图展示了 HTTP OTA 升级方式的过程： 

![img](https://www.rt-thread.org/qa/data/attachment/forum/201903/31/103742vauewcywcyqunpap.gif) 
