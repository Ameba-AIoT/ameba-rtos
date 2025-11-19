# Example Description

This example read a specific flash offset, modify it and re-read again by MBED API.

# HW Configuration

Nor Flash

# SW Configuration

Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expected Result

If the test passed, the log will be printed:

```
Read Data 0x13572468
Read Data 0x13572468
Result is success
```

# Note

* User should set flash test start address (`FLASH_APP_BASE`) carefully to avoid corrupting image section.

# Supported IC

- AmebaSmart`(with Nor Flash)`
  * RTL8730E
- AmebaLite
  * RTL8726E
  * RTL8720E
  * RTL8713E
  * RTL8710E
- AmebaDplus
  * RTL8721Dx
