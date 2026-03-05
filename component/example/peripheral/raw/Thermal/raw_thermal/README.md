# Example Description

1. This example describes how to get the current temperature in Celsius and Fahrenheit formats.

# HW Configuration

None.

# SW Configuration

1. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expected Result

1. The temperature values will be printed out periodically in the following format:
   ```
   Temperature: +25.1234
   Temperature = 25.1234 C
   Temperature = 77.1234 F
   ```
2. If temperature threshold is reached, corresponding interrupt message will be printed:
   - `Low warning temperature detect interrupt`
   - `High warning temperature detect interrupt`

# Note

none

# Supported IC

RTL8730E
RTL8726E
RTL8720E
RTL8713E
RTL8710E
RTL8721F
