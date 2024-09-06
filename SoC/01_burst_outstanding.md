# Outstanding Concept

## what's outstanding means in AXI bus？

In the context of the AXI (Advanced eXtensible Interface) bus, which is a part of the ARM AMBA (Advanced Microcontroller Bus Architecture), "outstanding" generally refers to transactions that have been initiated but not yet completed.

In a typical synchronous bus, each read or write transaction must be completed before the next one can be initiated. This means that the bus is idle during the time it takes for the data to be fetched or written, which can be inefficient.

In an AXI bus, however, multiple transactions can be initiated without waiting for the previous transactions to complete. This is known as having "outstanding" transactions. This allows for more efficient use of the bus, as it can remain busy while data is being fetched or written.

For instance, an outstanding read transaction means that the read address has been issued, but the corresponding read data has not yet been returned. Similarly, an outstanding write transaction means that the write address and data have been issued, but the acknowledgment has not yet been received.

## how to understand the number of outstanding in AXI?

The number of outstanding transactions in an AXI bus refers to the number of transactions that have been initiated but not yet completed. This number can be viewed as a measure of the bus's utilization and efficiency.

In an AXI bus, multiple read or write transactions can be initiated without waiting for previous transactions to complete. This allows for more efficient use of the bus, as it can remain busy while data is being fetched or written.

The maximum number of outstanding transactions that an AXI bus can handle at once is determined by the AXI protocol and the specific implementation of the AXI bus. For example, the AXI4 protocol allows for up to 16 outstanding transactions.

However, having too many outstanding transactions can also lead to complications. For example, if there are too many outstanding read transactions, the read data can arrive out of order, which can complicate the design of the system that is receiving the data.

Therefore, the number of outstanding transactions is something that system designers need to consider and manage carefully to balance efficiency and complexity.

## does apu's cache has outstanding concept?

Yes, the concept of "outstanding" transactions can also apply to a CPU's (or APU's) cache.

In this context, an "outstanding" transaction typically refers to a memory operation (such as a read or write) that has been initiated by the CPU or APU but has not yet completed. For example, when the CPU or APU issues a read request to the cache, and the requested data is not in the cache (a cache miss), the cache has to fetch the data from the main memory. During this time, the read request is considered "outstanding."

The CPU or APU can have multiple outstanding cache transactions at the same time. This is especially common in multi-level cache architectures and multi-core processors, where multiple cache transactions can be in progress simultaneously.

The maximum number of outstanding transactions a CPU or APU can handle at once depends on its specific architecture and design. Having multiple outstanding transactions can improve performance by allowing the CPU or APU to do other work while waiting for the memory operation to complete. However, as with the AXI bus, managing the number of outstanding transactions is a trade-off between efficiency and complexity.

### burst 和 outstanding 的区别

Burst 和 Outstanding 是芯片设计中两个不同但相关的概念，主要用于描述数据传输和内存访问的特性：

Burst（突发传输）:

- 定义：Burst 是指在一次总线事务中连续传输多个数据单元的能力。

- 目的：提高数据传输效率，减少总线事务的开销。

特点：

- 只需要一次地址传输，就可以连续传输多个数据。

- 通常用于顺序访问内存或设备寄存器。

- 可以显著提高带宽利用率。

Outstanding（未完成请求）:

定义：Outstanding 指的是同时发起但尚未完成的请求数量。

目的：通过允许多个未完成的请求同时存在，来隐藏延迟并提高总体性能。

特点：

- 允许系统在等待一个请求完成的同时发起新的请求。

- 可以更好地利用系统资源，特别是在存在长延迟操作时。

- 通常需要更复杂的控制逻辑来管理多个未完成的请求。

主要区别：

功能focus：

- Burst 专注于单次事务中的连续数据传输。

- Outstanding 关注同时处理多个独立请求的能力。

性能提升方式：

- Burst 通过减少地址传输次数来提高效率。

- Outstanding 通过并行处理多个请求来隐藏延迟。

实现复杂度：

- Burst 相对简单，主要涉及数据传输控制。

- Outstanding 通常更复杂，需要管理多个并发请求。

应用场景：

- Burst 适合连续数据访问，如DMA传输。

- Outstanding 适合有多个独立操作的系统，如现代处理器的内存访问。

在实际设计中，这两个特性常常结合使用，以最大化系统性能。例如，一个系统可能支持多个outstanding的burst传输，既利用了burst的高效率，又通过outstanding机制隐藏了延迟。
