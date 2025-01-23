# Example Description
This example describes how to build and use RDP, Which shows how to call secure entry function(rdp_protection_entry) from NS functions and how to call NS function(no_protection_func) from secure functions.

# HW Configuration
None

# SW configuration
The steps to build RDP image are as following:
1. Copy `copy_files\secure_src.c` to `component\soc\chipname\img3\` and override the original one.
2. ./menuconfig.py to enable TrustZone
3. ./build.py -a raw_rdp to rebuild project

# Expect result
loguart output: `rdp call succeed!`

# Note
If OTP is not programmed to enable RDP, Bootloader will not load RDP Image and can not run this example. The steps to configure RDP and download RDP image can be found in "Chapter RDP, Application Note(shall be NDA version).pfd".

# Supported IC
* AmebaLite
* AmebaDplus
