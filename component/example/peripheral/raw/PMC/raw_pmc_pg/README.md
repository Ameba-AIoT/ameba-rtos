# Example Description

This example shows how to use freertos tickless with aontimer interruptable interface.

# HW Configuration


# SW Configuration

Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expected Result

IC will enter PG mode, and wakeup after 2s. The wakeup source is aontimer.

# Note

1. Freertos enter tickless if the wakelock bit map is 0. By default there is PMU_OS keep system awake.So we need release this PMU_OS to enable tickless mode.
2. Keep alive time is adjustable by parameter of pmu_set_sysactive_time(), unit is ms.
3. This example will not run in loop, which means it will keep active after wake up once.


# Supported IC

* AmebaSmart
* AmebaLite
* AmebaDplus
