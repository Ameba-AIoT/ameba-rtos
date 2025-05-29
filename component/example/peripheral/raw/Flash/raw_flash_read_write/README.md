# Example Description

This example will read a specific flash offset, erase sector, write Flash in `USER mode` and re-read again. Cycle ten times.

# HW Configuration

None

# SW Configuration

cmd: `./build.py -a raw_flash_read_write` to build image.

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

* AMEBASMART`(with Nor Flash)`
* AMEBALITE
* AMEBADPLUS
