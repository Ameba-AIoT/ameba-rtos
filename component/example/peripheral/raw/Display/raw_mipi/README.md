# Example Description
This example describes how to build and use MIPI, Which shows how to display picture.

# HW Configuration
`PA_14` shall connect to LCM reset pin During the PCB design phase.

# SW configuration
- How to compile :
  - perform cmd: './build.py -a raw_mipi' in directory {sdk_path}/amebaXXX_gcc_project

# Expect result
There are five pictures on the screen that keep switching.

# Note
* The Driver for LCM is based on ST7701S, where the screen resolution is 480*800.
* `DDR_AUTOGATING` in `component\soc\amebasmart\fwlib\include\ameba_boot.h` shall be disabled to avoid dram in self-refresh.

# Supported IC
* AmebaSmart
