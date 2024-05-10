# Example Description
This example describes how to build and use MIPI, Which shows how to display picture.

This example does not support the loguart command response.

# HW Configuration
`PA_14` shall connect to LCM reset pin During the PCB design phase.

# SW configuration
The steps to build MIPI image are as following:
1. Copy `main.c` to override `amebasmart_gcc_project\project_xx\src\main.c`
2. Copy `ameba_mipi_show.c` and `ameba_mipi_display.h` to `amebasmart_gcc_project\project_xx\src\`

# Expect result
There are five pictures on the screen that keep switching.

# Note
* The Driver for LCM is based on ST7701S, where the screen resolution is 480*800.
* `DDR_AUTOGATING` in `component\soc\amebasmart\fwlib\include\ameba_boot.h` shall be disabled to avoid dram in self-refresh.

# Supported IC
* AmebaSmart
