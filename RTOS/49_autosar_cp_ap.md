# AutoSAR CP AP

`Classic AutoSAR` 是基于强实时性的嵌入式 OS 上开发出来的软件架构，能满足传统汽车定制化的功能需求，且能很好胜任；但是一旦要汽车接入网络，网络很可能有延迟、干扰，很可能无法满足强实时性。

这种情况下 `Classic AutoSAR` 就无能为力了，所以就需要一套能够满足非实时性的架构系统软件，在这样的背景下，Adaptive AUTOSAR 就诞生了。但是由于 `Adaptive AUTOSAR` 安全级别基本还停留在 ASIL-B（最高是D），所以很多需要强安全级别的ECU当下还是需要 `Classic AutoSAR`（能满足ASIL-D）来实现。

## CP

基于 `Classic AutoSAR` 平台开发的汽车控制器，具有如下特点：

- 硬实时，可在us时间内完成事件的实时处理，硬实时任务必须满足最后期限的限制，以保证系统的可靠运行
- 高功能安全等级，其可达到ASIL－D的安全等级
- 对CPU、RAM或Flash等资源具有较低的占用率
- 软件功能通常是固化不可动态变更的

## AP

`Apdative Autosar` 作为异构软件平台的软件架构，主要用于域控制器，可以成为连接 `Classic AutoSAR` 和 `Linux` 这样的非实时 OS 的桥梁，其具有如下特点：

- 软实时，具有毫秒级内的最后期限，且偶尔错过最后期限也不会造成灾难性后果
- 具有一定的功能安全要求，可达到 ASIL－B 或更高
- 与经典平台不同的是，它更适用于多核动态操作系统的高资源环境，如 QNX

`Adaptive Autosar` 与 `Classic Autosar` 相比，虽实时性要求有所降低，但在保证一定功能安全等级的基础上，大大提高了对高性能处理能力的支持，以支持智能互联应用功能的开发，因此`C++` 将成为 `Adaptive Autosar` 平台的主要开发语言。

## 总结

`Adaptive Autosar` 的出现是为了在`Classic Autosar`平台基础上，针对不同的应用场景实现两者的共存和协作，`Classic Autosar` 平台支持高安全性和高实时性的应用场景，因此对于深度嵌入式的软件功能需部署运行在经典平台上。而 `Adaptive Autosar` 则支持大数据的并行处理，所以对于高性能运算的功能则需要运行在Adaptive平台上。

随着无人驾驶技术的如火如荼，车联网及万物互连、云技术的日益发展，`Adaptive Autosar` 的出现不仅可满足现有需求，还可满足未来汽车技术的革新变化，由于其支持各种自适应的部署、复杂的微控制器以及各种非 `Auosar` 系统的互动，未来汽车将拥有不同类型的架构并互相进行补充。