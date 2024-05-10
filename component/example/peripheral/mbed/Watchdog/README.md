# Example Description
This example describes how to use watchdog by MBED API. In this example, watchdog is setup to 5s timeout. Watchdog won't bark if we refresh it before timeout. The timer is also reloaded after refresh. Otherwise it will restart system in default .

This example does not support the loguart command response.

# HW Configuration
None

# SW configuration
The steps to build Watchdog image are as following:
1. Copy `main.c` to override `project\realtek_chipname_va0_example\src\src_xx\main.c`

# Expect result
1. After power-on and running for a period of time, the watchdog will be triggered to reset the entire SOC.
2. After reset, loguart output: `watchdog mbed example exec succeed!`

# Note
1. If the linker reports an error, please enable MBED_API in menuconfig.
2. Different cores can set and trigger different watch dogs.

# Supported IC
* AmebaSmart
* AmebaLite
* AmebaDplus
