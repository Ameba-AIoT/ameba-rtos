# Example Description

This example will read a specific flash offset, erase block, write Flash in `USER mode` and re-read again.

# HW Configuration

None

# SW Configuration

- Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expected Result

The log below :
```
NAND FLASH DEMO
Bad block scan!
Bad block scan done!
NAND FLASH DEMO Done!
```

# Note

User should set flash test start address (`FLASH_TEST_BASE_BLOCK`) carefully to avoid corrupting image section.

# Supported IC

RTL8735C