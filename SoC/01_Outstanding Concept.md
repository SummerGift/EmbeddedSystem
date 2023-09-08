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
