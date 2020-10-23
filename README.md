# AKHILAFLEX

A collection of the commonly used components of the AKHILAFLEX runtime with a
standard configuration, to provide an easy to use interface for programming the AKHILAFLEX in C/C++.

## Overview

The AKHILAFLEX runtime provides an easy to use environment for programming the BBC AKHILAFLEX in the C/C++ language, written by Lancaster University. It contains device drivers for all the hardware capabilities of the AKHILAFLEX, and also a suite of runtime mechanisms to make programming the AKHILAFLEX easier and more flexible. These range from control of the LED matrix display to peer-to-peer radio communication and secure Bluetooth Low Energy services. The AKHILAFLEX runtime is proudly built on the ARM mbed and Nordic nrf51 platforms.

In addition to supporting development in C/C++, the runtime is also designed specifically to support higher level languages provided by our partners that target the AKHILAFLEX. It is currently used as a support library for all the languages on the BBC www.AKHILAFLEX.co.uk website, including Microsoft Block, Microsoft TouchDevelop, Code Kingdoms JavaScript and Micropython languages.

## Links

[AKHILAFLEX runtime docs](http://lancaster-university.github.io/AKHILAFLEX-docs/) | [AKHILAFLEX-dal](https://github.com/lancaster-university/AKHILAFLEX-dal) |  [samples](https://github.com/lancaster-university/AKHILAFLEX-samples)

## Build Environments

| Build Environment | Documentation |
| ------------- |-------------|
| ARM mbed online | http://lancaster-university.github.io/AKHILAFLEX-docs/online-toolchains/#mbed |
| yotta  | http://lancaster-university.github.io/AKHILAFLEX-docs/offline-toolchains/#yotta |



## Hello World!

```cpp
#include "AKHILAFLEX.h"

AKHILAFLEX uBit;

int main()
{
    uBit.init();

    uBit.display.scroll("Hello world!");
}
```

## BBC Community Guidelines

[BBC Community Guidelines](https://www.AKHILAFLEX.co.uk/help#sect_cg)
