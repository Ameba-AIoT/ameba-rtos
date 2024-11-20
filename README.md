# ameba-rtos

* [中文版](./README_CN.md)

ameba-rtos is the development framework for Realtek SoCs supported on Linux.

# Supported SoCs

|Chip         |          master       |     release/v1.0       |
|:----------- |:---------------------:| :---------------------:|
|AmebaSmart   |![alt text][supported] | ![alt text][supported] |
|AmebaLite    |![alt text][supported] | ![alt text][supported] |
|AmebaDplus   |![alt text][supported] | ![alt text][supported] |

[supported]: https://img.shields.io/badge/-supported-green "supported"

# Documentation

Documentation for latest version: https://github.com/Ameba-AIoT/ameba-iot-docs. Click the URLs listed in README to view the documentations of the specific SoC.

**Note:** Each SoC series has its own documentation, please find documentation with the specified chip.

# Accelerate with Gitee

For users who can access [Gitee](https://gitee.com), we recommend that import the repository into Gitee to improve download speed if you find the download speed from GitHub is too slow.

The detailed steps please refer to [How to import third-party Git repositories such as GitHub](https://gitee.com/help/articles/4261). 

# Quick Reference

See the ApplicationNote chapter **Build Environment** from above links for a detailed setup guide. This is a quick reference for common commands working with ameba-rtos projects:

## Setup Build Environment

* The toolchain will be intalled in `/opt/rtk-toolchain` by default. If an error "Error: No Toolchain in `/opt/rtk-toolchain/vsdk-10.3.1/linux/newlib`" encounters when building the project, please refer to ApplicationNote section **Installing Toolchain** for more details.
* Linux platform is supported for now, Ubuntu version 16.04 64-bit or higher is required.
* Windows built with VScode will be supported in the future.
* Python version 3.7 or higher is required. Run `python --version` to check the version. If an error "Command `python` not found" encounters, please refer to ApplicationNote section **Preparing GCC Environment** install python3. If still error appears, please run `sudo ln -s /usr/bin/python3 /usr/bin/python` to create symbolic link from `/usr/bin/python3` to `/usr/bin/python`.

**Note:** The toolchain will be downloaded from GitHub when build project for the first time. If find the download speed from GitHub is too slow or download failed, please execute command `make toolchain URL=aliyun` or `make toolchain URL=github` first to get toolchain before building project. We recommend use `make toolchain URL=aliyun` to download toolchain from aliyun to improve the download speed.

## Finding the Project

ameba-rtos support projects with the supported SoCs listed as above with the naming rule amebaxxx_gcc_project.

Once you've found the project you want to work with, change to its directory and you can configure and build it.

## Configuring the Project

Run `make menuconfig` to open a configuration menu and you can configure the project options.

* General Config: the shared kernel configurations for all cores.
* Network Config: the incompatible kernel configurations for all cores.
* Core Config: the exclusive kernel configurations for each core.

## Compiling the Project

* For normal image, simply run `make all` command to build SDK.
* For compiling examples, run `make all EXAMPLE=xxx`, refer to readme.txt in each [example](component/example) directory for more details.
* For MP image, refer to Section How to Build MP Image to build SDK.

After compiling finishes, both app and bootloader images are generated.

**Note:** If somehow failed, type `make clean` to clean and then redo the make procedure.

## Flashing

When the build finishes, downloading images into flash by [AmebaImageTool](tools/ameba/ImageTool/AmebaImageTool.exe):

See the ApplicationNote chapter **Image Tool** from documentation links for more details.

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

* If you have any issues or suggestions during project development, please login [RealMCU](https://www.realmcu.com/en/Account/Login?ReturnUrl=%2FCommunity%2Fcima%2F3187aedd-cb0d-444b-aa0c-284fd82cc501) and give feedback.

* If you find a bug or have a feature request, please [Check the Issues section on github](https://github.com/Ameba-AIoT/ameba-rtos/issues). Please check existing Issues before opening a new one.