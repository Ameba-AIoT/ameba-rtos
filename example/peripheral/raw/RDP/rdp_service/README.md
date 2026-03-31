# Example Description
This example demonstrates RDP (Runtime Data Protection) service:
- Call secure entry function from non-secure world
- Pass protected data to non-secure callback for processing

# Architecture
RDP service is implemented in `component/soc/common/image3/secure_src.c`:

| API | Description |
|-----|-------------|
| `rdp_service_init(callback)` | Register callback for later use |
| `rdp_secure_compute(idx)` | Compute result in secure world |
| `rdp_callback_process(idx)` | Pass data to registered callback |

Include `rdp_service_example.h` to use these APIs.

# HW Configuration
Target chip must support TrustZone. Error log appears if not supported.

# SW Configuration
1. Build:
   ```bash
   python ameba.py build -a rdp_service
   ```
2. Download images via Ameba Image Tool.

# Expected Result
```
[RDP-I] === RDP Service Demo ===
[RDP-I] idx=0: secure=0xXXXXXXXX, callback=0xXXXXXXXX OK
[RDP-I] idx=1: secure=0xXXXXXXXX, callback=0xXXXXXXXX OK
[RDP-I] idx=2: secure=0xXXXXXXXX, callback=0xXXXXXXXX OK
[RDP-I] idx=3: secure=0xXXXXXXXX, callback=0xXXXXXXXX OK
[RDP-I] Demo complete!
```

# Note
OTP must be programmed to enable RDP, otherwise bootloader won't load RDP image.

# Supported IC

RTL8726E
RTL8720E
RTL8713E
RTL8710E
RTL8721Dx
RTL8721F
