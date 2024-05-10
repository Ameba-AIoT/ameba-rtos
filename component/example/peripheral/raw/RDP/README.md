# Example Description
This example describes how to build and use RDP, Which shows how to call secure entry function(rdp_protection_entry) from NS functions and how to call NS function(no_protection_func) from secure functions.

This example does not support the loguart command response.

# HW Configuration

None

# SW configuration


The steps to build RDP image are as following:
1. Copy `main.c` to override `project\realtek_chipname_va0_example\src\src_xx\main.c` (Only KM4)
2. Copy `secure_src.c` to `component\soc\chipname\img3\` and override the original one.
3. make menuconfig to enable TrustZone and rebuild project

# Expect result

loguart output: `rdp call succeed!`

# Note

If OTP is not programmed to enable RDP, Bootloader will not load RDP Image and can not run this example. The steps to configure RDP and download RDP image can be found in "Chapter RDP, Application Note(shall be NDA version).pfd".

# Supported IC

* AmebaLite
* AmebaDplus
