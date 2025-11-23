# Example Description

This example demonstrates how to configure the status register of Winbond to protect all blocks by MBED API. Note that the status register should be reset if users want to reload code.

# HW Configuration

Winbond Nor Flash

# SW Configuration

Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expected Result

If the test passed, the log will be printed:

```
Test Address = 180000
Before Lock
Success 1
Lock
Status Register Before Setting: 0x00
Success 2
Unlock
Status Register After Setting: 0x1c
Success 3
Status Register After Reset: 0x00
Result is success
```

# Note

* This example is for `Winbond Flash`. Not necessarily compatible with Flash chips from other manufacturers. Please modify the status register settings according to the flash data sheet. 
* User should set flash test start address (`FLASH_APP_BASE`) carefully to avoid corrupting image section.

# Supported IC

- AmebaSmart`(with Nor Winbond Flash)`
  * RTL8730E
- AmebaLite
  * RTL8726E
  * RTL8720E
  * RTL8713E
  * RTL8710E
- AmebaDplus
  * RTL8721Dx
