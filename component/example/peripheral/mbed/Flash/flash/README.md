# Example Description

This example read a specific flash offset, modify it and re-read again by MBED API.

This example does not support the loguart command response.

# HW Configuration

Nor Flash

# SW Configuration

Replace the original main.c under sdk project (project\realtek_amebaxxx_va0_example\src\src_xxx) with the main.c in the example directory. Then re-build project. 

For AmebaSmart, please compile it in CA32(ap) core. 

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

* AMEBASMART`(with Nor Flash)`
* AMEBALITE
* AMEBADPLUS

