> **Before selecting pins**, refer to the [Peripheral Pin Usage Guide](../../../README.md).

# Example Description

1. This example describes how to get the current temperature in Celsius and Fahrenheit formats.

# HW Configuration

1. none

# SW Configuration

1. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expected Result

1. After boot, the current temperature is printed once in three forms, e.g.:

   ```
   Temperature: +25.1234 C
   Temperature = 25.123400 C
   Temperature = 77.222122 F
   ```

   * String type: `TM_Display_Result(u32 Data)`, built on `TM_GetDegree(Data, TM_DEG_C)`
   * Celsius type: `TM_GetDegree(u32 Data, TM_DEG_C)`
   * Fahrenheit type: `TM_GetDegree(u32 Data, TM_DEG_F)`

2. Thresholds are set in Celsius: low warning 20, high warning 50, high protect 125. When the temperature crosses a warning threshold, the interrupt handler prints the warning and the current temperature (the first line is printed only for the threshold that triggered):

   ```
   Low warning temperature detect interrupt
   High warning temperature detect interrupt
   Temperature: +52.3456 C
   ``

# Note

none

# Supported IC

RTL8730E
RTL8726E
RTL8720E
RTL8713E
RTL8710E
RTL8721F
RTL8720F
