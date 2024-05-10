# Example Description

1. This example describes how to get the current temperature in Celsius and Fahrenheit formats.

This example does not support the loguart command response.

# HW Configuration

1. none

# SW Configuration

1. How to compile :
   	(1) copy main.c under folder src to project\realtek_amebaxxx_va0_example\src\src_xxx

   ​	(2) make and download new image.

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