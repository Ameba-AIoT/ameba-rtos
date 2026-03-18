# Example Description
This example demonstrates how to use RDP (Runtime Data Protection) service, which shows:
- How to call secure entry function (`rdp_protection_entry`) from non-secure functions
- How to call non-secure function (`no_protection_func`) from secure functions via callback

# Architecture
The RDP service is implemented in `component/soc/amebagreen2/img3/rdp_service_example.c` and provides:
- `rdp_protection_entry()`: Secure entry function for protected computation
- `rdp_no_protection_call()`: Secure function that calls non-secure callback

The header file `component/soc/amebagreen2/img3/include/rdp_service_example.h` provides the public API.

# HW Configuration
The target chip must support TrustZone feature. Otherwise, an error log will be printed: "OTP is wrong, this chip does not support rdp."

# SW Configuration
The steps to build RDP image are as following:
1. Build:
   ```bash
   python ameba.py build -a rdp_service
   ```
2. Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * Download images to board by Ameba Image Tool.

# Expect result
Loguart output:
```
[RDP_SERVICE-I] === RDP Service Demo Start ===
[RDP_SERVICE-I] === RDP Service Demo Complete ===
[RDP_SERVICE-I] All 32 tests passed! RDP call succeed!
```

# Note
If OTP is not programmed to enable RDP, Bootloader will not load RDP Image and cannot run this example. The steps to configure RDP and download RDP image can be found in RDP chapter.

# Supported IC

RTL8726E
RTL8720E
RTL8713E
RTL8710E
RTL8721Dx
RTL8721F
