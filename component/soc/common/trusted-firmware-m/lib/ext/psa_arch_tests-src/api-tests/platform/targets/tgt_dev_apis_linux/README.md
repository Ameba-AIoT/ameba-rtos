# Linux Host Target

There are a couple of limitations to this target when it comes to a test which involves system reset or test process due to Watch Dog Timer (WDT) and NVMEM implementation.

- **WDT**:  Lacks functionality to recover after a hang as they just return failure or success.

- **NVMEM**: Stores data in an array in memory, which means NVMEM would be lost as it isn't a non-volatile implementation.

## License

Arm PSA test suite is distributed under Apache v2.0 License.

--------------

*Copyright (c) 2021, Arm Limited and Contributors. All rights reserved.*
