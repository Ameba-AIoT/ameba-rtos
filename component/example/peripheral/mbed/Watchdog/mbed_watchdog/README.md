# Example Description
This example describes how to use watchdog by MBED API. In this example, watchdog is setup to 5s timeout. Watchdog won't bark if we refresh it before timeout. The timer is also reloaded after refresh. Otherwise it will restart system in default .

# HW Configuration
None

# SW configuration

Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result
1. After power-on and running for a period of time, the watchdog will be triggered to reset the entire SOC.
2. After reset, loguart output: `watchdog mbed example exec succeed!`

# Note
1. If the linker reports an error, please enable MBED_API in menuconfig.
2. Different cores can set and trigger different watch dogs.

# Supported IC
- AmebaSmart
  * RTL8730E
- AmebaLite
  * RTL8726E
  * RTL8720E
  * RTL8713E
  * RTL8710E
- AmebaDplus
  * RTL8721Dx
