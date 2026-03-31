# CA32 Trustzone Example Description

This example demonstrates how to invoke secure world services from the non-secure world on the CA32 (ARM Cortex-A32) processor.

## Overview

ARM Trustzone technology divides the system into two isolated worlds:
- **Secure World**: Runs trusted code, handles sensitive data and operations
- **Non-Secure World**: Runs normal applications

The non-secure world uses the SMC (Secure Monitor Call) instruction to enter the secure world and request secure services.

## Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    Non-Secure World (EL0/EL1)               │
│  ┌─────────────────────────────────────────────────────┐    │
│  │  example_CA32Trustzone.c                            │    │
│  │  - invoke_securetest() initiates SMC call           │    │
│  │  - Function ID: 0x82000001                          │    │
│  └─────────────────────────────────────────────────────┘    │
└──────────────────────────┬──────────────────────────────────┘
                           │ SMC instruction
                           ▼
┌─────────────────────────────────────────────────────────────┐
│                    Secure Monitor (EL3)                     │
│                    ARM Trusted Firmware (ATF)               │
└──────────────────────────┬──────────────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────────────┐
│                    Secure World (EL1S)                      │
│  ┌─────────────────────────────────────────────────────┐    │
│  │  bsec_svc.c (Secure service implementation)         │    │
│  │  - rtk_secure_service() handles the request         │    │
│  │  - Prints "secure test here"                        │    │
│  │  - Returns test value 0x5a5a                        │    │
│  └─────────────────────────────────────────────────────┘    │
└─────────────────────────────────────────────────────────────┘
```

## Source Files

| File | Location | Description |
|------|----------|-------------|
| example_CA32Trustzone.c | example/peripheral/raw/CA32Trustzone/ | Non-secure world code, initiates SMC call |
| app_example.c | example/peripheral/raw/CA32Trustzone/ | Application entry point |
| bsec_svc.c | component/soc/amebasmart/atf/plat/realtek/sheipa/service/ | **Secure world code**, implements rtk_secure_service() |

## Secure World Code Location

The secure service is implemented in ATF (ARM Trusted Firmware):

```
component/soc/amebasmart/atf/plat/realtek/sheipa/service/bsec_svc.c
```

## HW Configuration

No additional hardware configuration required.

## SW Configuration

Build and Download:
1. Refer to the SDK Examples section of the online documentation to generate images.
2. `Download` images to board by Ameba Image Tool.

### Build Commands (RTL8730E)

```bash
# Set target SoC
python ameba.py soc RTL8730E

# Build example
python ameba.py build -a CA32Trustzone -p
```

Build output is located at: `build_RTL8730E/`

## Expected Result

After running, the serial output will show:

```
secure test here           <- From secure world (ATF)
value: 0 0x5a5a 0          <- From non-secure world (return code, ret_secure_value, reserved)
```

- `secure test here`: Printed by `rtk_secure_service()` in the secure world
- `value: 0 0x5a5a 0`:
  - `0`: RTK_SMC_SUCCESS (call succeeded)
  - `0x5a5a`: ret_secure_value (test value returned from secure world)
  - `0`: Reserved field

## Note

1. This example only supports RTL8730E (amebasmart) as it requires CA32 processor support
2. Secure world code is part of ATF and will be packaged into the firmware during build
3. To add custom secure services, modify `rtk_secure_service()` in `bsec_svc.c`

## Supported IC

RTL8730E
