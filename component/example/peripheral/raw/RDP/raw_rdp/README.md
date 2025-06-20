# Example Description
This example describes how to build and use RDP, Which shows how to call secure entry function(rdp_protection_entry) from NS functions and how to call NS function(no_protection_func) from secure functions.

# HW Configuration
None

# SW configuration
The steps to build RDP image are as following:
1. ./menuconfig.py to enable TrustZone.
2. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result
loguart output: `rdp call succeed!`

# Note
If OTP is not programmed to enable RDP, Bootloader will not load RDP Image and can not run this example. The steps to configure RDP and download RDP image can be found in "Chapter RDP, Application Note(shall be NDA version).pfd".

# Supported IC
* AmebaLite
* AmebaDplus
