# ameba-rtos

ameba-rtos is the development framework for Realtek SoCs supported on Linux.

# Supported SoCs

|Chip         |          master       |     release/v1.0       |
|:----------- |:---------------------:| :---------------------:|
|AmebaSmart   |![alt text][supported] | ![alt text][supported] |
|AmebaLite    |![alt text][supported] | ![alt text][supported] |
|AmebaDplus   |![alt text][supported] | ![alt text][supported] |

[supported]: https://img.shields.io/badge/-supported-green "supported"

# Documentation

Documentation for latest version: https://www.realmcu.com/. Get application note and datasheet depending on chip you use.

**Note:** Each SoC series has its own documentation, please find documentation with the specified chip.

# Quick Reference

See the ApplicationNote chapter **Build Environment** from above links for a detailed setup guide. This is a quick reference for common commands working with ameba-rtos projects:

## Setup Build Environment

* Linux platform is supported for now, Ubuntu version 16.04 64-bit or higher is required.
* Windows built with VScode will be supported in the future.
* Python version 3.7 or higher is required.

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

If you have any issues or suggestions during project development, please login [RealMCU](https://www.realmcu.com/en/Account/Login?ReturnUrl=%2FCommunity%2Fcima%2F3187aedd-cb0d-444b-aa0c-284fd82cc501) and give feedback.
