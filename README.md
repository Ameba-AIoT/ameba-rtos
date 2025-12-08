# ameba-rtos

* [中文版](./README_CN.md)

ameba-rtos is the development framework for Realtek SoCs supported on Linux and Windows.

# Supported SoCs

| Chip      |          master       |     release/v1.0       |     release/v1.1       |
|:--------- |:---------------------:| :---------------------:| :---------------------:|
| RTL8730E  |![alt text][supported] | ![alt text][supported] | ![alt text][supported] |
| RTL8726E  |![alt text][supported] | ![alt text][supported] | ![alt text][supported] |
| RTL8721Dx |![alt text][supported] | ![alt text][supported] | ![alt text][supported] |
| RTL8710E  |![alt text][supported] | ![alt text][supported] | ![alt text][supported] |
| RTL8721F  |![alt text][supported] |                        |                        |
| RTL8720E  |![alt text][supported] | ![alt text][supported] | ![alt text][supported] |
| RTL8713E  |![alt text][supported] | ![alt text][supported] | ![alt text][supported] |

[supported]: https://img.shields.io/badge/-supported-green "supported"

# Documentation

Documentation for latest version: [RealMCU](https://aiot.realmcu.com/en/latest/rtos/index.html). Select a product from the menu bar to view the documentations of the specific SoC.

**Note:** Each SoC series has its own documentation, please find documentation with the specified chip.

# Accelerate with Gitee

For users who can access [Gitee](https://gitee.com), we recommend that download the Gitee repository [ameba-rtos](https://gitee.com/ameba-aiot/ameba-rtos) to improve download speed if you find the download speed from GitHub is too slow.

# Quick Reference

We support the following two methods for configuring the build environment:

## Using the VS Code Extension Plugin

The Ameba extension plugin is developed based on Cline and is fully compatible with all native Cline features. It provides enhanced support
specifically for Realtek Ameba series chips, including:

* Automatic environment checking and installation
* Automated SDK configuration
* One-click project compilation
* Convenient flashing tools
* Integrated serial port monitoring

Refer to the [VS Code User Guide](https://aiot.realmcu.com/en/latest/tools/vscode/index.html) for plugin installation instructions.
After installing the plugin, you can efficiently complete environment setup, project compilation, flashing, and serial port monitoring.

## Manual Build Environment Configuration

If you prefer to configure the build environment manually, please refer to the [FreeRTOS SDK User Guide](https://aiot.realmcu.com/en/latest/rtos/sdk/index.html).
Follow the steps described in the guide to:

* Set up the SDK environment
* Compile your project
* Flash the images
* Monitor the serial port

# Feedback

* If you have any issues or suggestions during project development, please login [Real-AIOT](https://forum.real-aiot.com/) and give feedback.

* If you find a bug or have a feature request, please [Check the Issues section on github](https://github.com/Ameba-AIoT/ameba-rtos/issues). Please check existing Issues before opening a new one.