# Example Description
This example describes how to build and use RDP, Which shows how to call secure entry function(rdp_protection_entry) from NS functions and how to call NS function(no_protection_func) from secure functions.

# HW Configuration
The target Chip must support Trustzone feature. Otherwise echo error log: "OTP is wrong, this chip does not support rdp."

# SW configuration
The steps to build RDP image are as following:
1. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool.

# Expect result
loguart output: `rdp call succeed!`

# Note
If OTP is not programmed to enable RDP, Bootloader will not load RDP Image and can not run this example. The steps to configure RDP and download RDP image can be found in RDP chapter.

# Supported IC

RTL8726E
RTL8720E
RTL8713E
RTL8710E
RTL8721Dx
