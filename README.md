# ameba-rtos

* [中文版](./README_CN.md)

ameba-rtos is the development framework for Realtek SoCs supported on Linux and Windows.

# Supported SoCs

|Chip         |          master       |     release/v1.0       |     release/v1.1       |
|:----------- |:---------------------:| :---------------------:| :---------------------:|
|AmebaSmart   |![alt text][supported] | ![alt text][supported] | ![alt text][supported] |
|AmebaLite    |![alt text][supported] | ![alt text][supported] | ![alt text][supported] |
|AmebaDplus   |![alt text][supported] | ![alt text][supported] | ![alt text][supported] |

[supported]: https://img.shields.io/badge/-supported-green "supported"

# Documentation

Documentation for latest version: [RealMCU](https://aiot.realmcu.com/docs/en/latest/index.html). Select a product from the menu bar to view the documentations of the specific SoC.

**Note:** Each SoC series has its own documentation, please find documentation with the specified chip.

# Accelerate with Gitee

For users who can access [Gitee](https://gitee.com), we recommend that download the Gitee repository [ameba-rtos](https://gitee.com/ameba-aiot/ameba-rtos) to improve download speed if you find the download speed from GitHub is too slow.

# Quick Reference

See the ApplicationNote chapter [GCC Build Environment](https://aiot.realmcu.com/docs/en/latest/rst_rtos/0_gcc_build_environment/1_gcc_build_environment_toprst.html) from above links for a detailed setup guide. This is a quick reference for common commands working with ameba-rtos projects:

## Setup Build Environment

* The toolchain will be intalled in `/opt/rtk-toolchain` by default. If an error "Error: No Toolchain in `/opt/rtk-toolchain/vsdk-10.3.1/linux/newlib`" encounters when building the project, please refer to ApplicationNote section [Installing Toolchain](https://aiot.realmcu.com/docs/en/latest/rst_rtos/0_gcc_build_environment/1_gcc_build_environment_toprst.html#installing-toolchain) for more details.
* Linux platform is supported, Ubuntu version 16.04 64-bit or higher is required.
* Windows platform is supported, Windows 10 64-bit is required.
* Python version 3.7 or higher is required. Run `python --version` to check the version. If an error "Command `python` not found" encounters, please refer to ApplicationNote section [Preparing GCC Environment](https://aiot.realmcu.com/docs/en/latest/rst_rtos/0_gcc_build_environment/1_gcc_build_environment_toprst.html#preparing-gcc-environment) install python3. If still error appears, please run `sudo ln -s /usr/bin/python3 /usr/bin/python` to create symbolic link from `/usr/bin/python3` to `/usr/bin/python`.

**Note:** The toolchain will be downloaded from GitHub when build project for the first time. If find the download speed from GitHub is too slow or download failed, please execute command `./build.py -D USE_ALIYUN_URL=true` first to get toolchain from aliyun to improve the download speed before building project.

## Finding the Project

ameba-rtos support projects with the supported SoCs listed as above with the naming rule amebaxxx_gcc_project.

Once you've found the project you want to work with, change to its directory and you can configure and build it.

## Configuring the Project

Run `./menuconfig.py` to open a configuration menu and you can configure the project options.

* General Config: the shared kernel configurations for all cores.
* Connectivity Config: the connectivity configurations for all cores.
* Core Config: the exclusive kernel configurations for each core.

## Compiling the Project

* For normal image, simply run `./build.py` command to build SDK.
* For compiling examples, run `./build.py -a xxx`, refer to readme.txt in each [example](component/example) directory for more details.
* For MP image, refer to Section [MP Image](https://aiot.realmcu.com/docs/en/latest/rst_mp/6_mass_production/1_mp_toprst.html) to build SDK.

After compiling finishes, both app and bootloader images are generated.

**Note:** If somehow failed, type `./build.py -p` to clean and then redo the build procedure.

## Flashing

When the build finishes, downloading images into flash by [AmebaImageTool](tools/ameba/ImageTool/AmebaImageTool.exe):

See the ApplicationNote chapter [Image Tool](https://aiot.realmcu.com/docs/en/latest/rst_tools/image_tool/1_image_tool_toprst.html) from documentation links for more details.

* Environment Requirements: EX. WinXP, Win 7 or later, Microsoft .NET Framework 4.0.
* Connect chip and PC with USB wire.
* Choose the Device profiles according to the chip you use.
* Select the corresponding serial port and transmission baud rate. The default baud rate is 1500000.
* Select the images to be programmed and set the start address and end address according to the flash layout, refer to [ameba_flashcfg.c/Flash_layout].
* Click the Download button and start. The progress bar will show the download progress of each image and the log window will show the operation status.

**Note:** For an empty chip, the bootloader and app image shall be downloaded.

## Erasing Flash

The image download only erase the flash fields with images programmed. If you want erase the entire flash or specified field, you can use the **Flash Erase** function of [AmebaImageTool](tools/ameba/ImageTool/AmebaImageTool.exe).

* Choose the Device profiles according to the chip you use.
* Select the corresponding serial port and transmission baud rate. The default baud rate is 1500000.
* Set the start address and size you want to erase.
* Click the Erase button and erase operation begins. You would get the operation result from the log window.

# Feedback

* If you have any issues or suggestions during project development, please login [Real-AIOT](https://forum.real-aiot.com/) and give feedback.

* If you find a bug or have a feature request, please [Check the Issues section on github](https://github.com/Ameba-AIoT/ameba-rtos/issues). Please check existing Issues before opening a new one.