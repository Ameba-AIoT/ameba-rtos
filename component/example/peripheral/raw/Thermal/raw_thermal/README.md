# Example Description

1. This example describes how to get the current temperature in Celsius and Fahrenheit formats.

# HW Configuration

1. none

# SW Configuration

1. How to compile :
    * Run `./build.py -a raw_thermal` under project path, e.g. `amebasmart_gcc_project/`, to generate images.
    * `Download` images to board by Ameba Image Tool.

# Expected Result

1. Three forms of temperature values can be obtained:
   String type: `TM_Display_Result(u32 Data)`
   Celsius type: `TM_GetCdegree(u32 Data)`
   Fahrenheit type:`TM_GetFdegree(u32 Data)`

# Note

none

# Supported IC

* AmebaSmart
* AmebaLite
