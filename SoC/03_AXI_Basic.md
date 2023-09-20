# AXI Basic

当代 SOC 越来越复杂，内部封装了多种 IP，这些 IP 通过系统总线相互连接，AXI 总线便是常用的总线之一。

## AXI 架构

AXI 协议是一种基于 burst 的协议，定义了一系列独立的传输通道：

- read address
- read data
- write address
- write data
- write response

地址通道运输控制信息，描述了将要被传输数据的种类。数据在 master 和 slave 之间被传输，通过下列通道之一：

- 写数据通道从 master 传输数据到 slave，在一次写传输中 slave 使用 write resonse channel 来通知传输完成
- 一个读数据通道用于传输数据从 slave 到 master

AXI 协议：

- 允许地址信息在实际的数据之前下发
- 支持 multiple outstanding transactions
- 支持 out-of-order 完成传输

### Read transaction

![image-20230919161824901](figures/image-20230919161824901.png)

### Write transaction

![image-20230919161852630](figures/image-20230919161852630.png)

### Channel definition

每一个独立的通道包含一组信息信号 VALID 和 READY，提供双向的握手机制。

source 使用 VALID 信号来指示通道上有有效地址数据或者控制信息，destination 使用 READY 信号来指示它可以接收这些信息。读和写通道都包含一个 LAST 信号来指示 the final item in transaction。

#### Read and write address channels

Read and write transactions each have their own address channel. The appropriate address channel carries all of the 

required address and control information for a transaction.

因为读和写数据的地址通道是独立的，因此在分析波形时，可以分别查看 araddr 和 awaddr 信号来查看 master 的读写地址。

#### **Read data channel**

The read data channel carries both the read data and the read response information from the slave to the master, and 

includes:

- the data bus, that can be 8, 16, 32, 64, 128, 256, 512, or 1024 bits wide

- a read response signal indicating the completion status of the read transaction.

读数据通路运输从 slave 发送到 master 的数据，还有一个指示 read transaction 完成的信号。

#### **Write data channel**

The write data channel carries the write data from the master to the slave and includes:

• the data bus, that can be 8, 16, 32, 64, 128, 256, 512, or 1024 bits wide

• a byte lane strobe signal for every eight data bits, indicating which bytes of the data are valid.

Write data channel information is always treated as buffered, so that the master can perform write transactions 

without slave acknowledgement of previous write transactions.

####  **Write response channel**

A slave uses the write response channel to respond to write transactions. All write transactions require completion 

signaling on the write response channel.

As Figure A1-2 on page A1-22 shows, completion is signaled only for a complete transaction, not for each data 

transfer in a transaction.

只有一次 transaction 完全完成时，才会发出 completion signal。
