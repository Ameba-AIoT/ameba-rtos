# Example Description

This example demonstrates how to wake the system from deepsleep mode using an aontimer.

# HW Configuration
None

# SW Configuration

Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expected Result

chip will wakeup from dslp after timeout, log will show "dslp wake from aontimer"

# Note

1. Because aontimer uses a 100kHz clock, the timer's precision is insufficient, resulting in a deviation in the actual wake-up time.
2. This example will not run in loop, which means it will keep active after wake up once.
3. If there is any information that needs to be saved before the system sleeps, it can be saved in the three registers BKUP1 ~ BKUP3. BKUP0 has a special purpose and cannot be used to store information.

# Supported IC

RTL8730E
RTL8726E
RTL8720E
RTL8713E
RTL8710E
RTL8721Dx
RTL8721F
