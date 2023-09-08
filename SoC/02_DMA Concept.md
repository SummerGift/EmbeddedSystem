# DMA Concept

## what's the different between brust size and brust len in dma?

In the context of Direct Memory Access (DMA), burst size and burst length are two different parameters that control how data is transferred between memory and a peripheral device.

1. **Burst Size**: This typically refers to the amount of data that can be transferred in a single transaction or operation. The burst size is usually defined in terms of bytes. For example, a burst size of 4 means that 4 bytes of data can be transferred in a single operation.

2. **Burst Length**: This refers to the number of data items (which can be bytes, words, or other units of data, depending on the system) that are transferred in a single burst. In other words, it's the number of times the burst size amount of data is transferred in one burst. For example, if the burst size is 4 bytes and the burst length is 8, then a total of 32 bytes of data will be transferred in a single burst.

The exact definitions and usage of these terms can vary depending on the specific DMA controller and system architecture. In some contexts, the terms might be used interchangeably or have slightly different meanings. Always refer to the specific documentation for the system or component you're working with for the most accurate information.
