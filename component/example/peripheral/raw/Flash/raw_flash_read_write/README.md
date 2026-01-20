# Example Description

This example will read a specific flash offset, erase sector, write Flash in `USER mode` and re-read again. Cycle ten times.

# HW Configuration

None

# SW Configuration

- Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expected Result

The log below repeats 10 times:
```
Read Data 0x13572468
Read Data 0x13572468
Result is success
```

# Note

User should set flash test start address (`FLASH_APP_BASE`) carefully to avoid corrupting image section.

# Supported IC

RTL8730E`(with Nor Flash)`
RTL8726E
RTL8720E
RTL8713E
RTL8710E
RTL8721Dx
RTL8721F