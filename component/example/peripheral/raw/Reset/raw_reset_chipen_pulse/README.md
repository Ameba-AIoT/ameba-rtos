# Example Description

This example shows how to use chipen pulse mode.

# HW Configuration

After the system boots, press and hold the `CHIP_EN` button for varying durations to trigger different scenarios.

# SW Configuration

Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expected Result

**1. Log Output:**
When the button press duration is shorter than LONG_PRESS_THRESHOLD_5S, the serial log will display:
> `short press event`

When the button press duration is longer than LONG_PRESS_THRESHOLD_5S, the serial log will display:
> `long press event`

# Note

# Supported IC

RTL8730E
RTL8726E
RTL8720E
RTL8713E
RTL8710E
RTL8721Dx
RTL8721F
