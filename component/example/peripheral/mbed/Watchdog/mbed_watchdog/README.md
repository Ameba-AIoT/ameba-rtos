# Example Description
This example describes how to use watchdog by MBED API. In this example, watchdog is setup to 5s timeout. Watchdog won't bark if we refresh it before timeout. The timer is also reloaded after refresh. Otherwise it will restart system in default .

# HW Configuration
None

# SW configuration
make and download new image

   - in {SDK}\amebaxxx_gcc_project path, input
   
   ```shell
   ./build.py -a mbed_watchdog
   ```

   - Use Realtek AmebaImageTool to download the image under auto_build folder to development board
   - press reset/chip_en button, then example will be start running.

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
