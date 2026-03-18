# Example Description
This example demonstrates ECDSA signing and verification across TrustZone boundaries:
- Secure world performs ECDSA signing
- Non-secure world performs ECDSA verification
- Shows how to pass data between secure and non-secure worlds

# HW Configuration
The target chip must support TrustZone feature.

# SW Configuration
The steps to build this example are as follows:
1. Build and Download:
   * Refer to the SDK Examples section of the online documentation to generate images.
   * `Download` images to board by Ameba Image Tool or ameba.py.

# Expected Result
Log output should show:
```
[ECDSA_TZ] === ECDSA TrustZone Demo Start ===
[ECDSA_TZ] Creating secure context for task...
[ECDSA_TZ] Message to sign: Hello from Non-Secure World!
[ECDSA_TZ] Calling secure world for signing...
[SECURE] === Secure ECDSA Sign Service ===
[SECURE] ECDSA key pair generated successfully
[SECURE] SHA256 hash computed
[SECURE] Signature created (len=71)
[SECURE] Public key exported (len=65)
[SECURE] === Secure Sign Complete ===
[ECDSA_TZ] Signature received from secure world (len=71)
[ECDSA_TZ] Public key length: 65
[ECDSA_TZ] Verifying signature in non-secure world...
[ECDSA_TZ] Non-secure verification succeeded!
[ECDSA_TZ] === ECDSA TrustZone Demo Complete ===
[ECDSA_TZ] Sign in Secure World, Verify in Non-Secure World: SUCCESS!
```

# Note
- This example uses `rtos_create_secure_context()` to allocate secure stack for the non-secure task
- The secure entry function `ecdsa_secure_sign()` is marked with `NS_ENTRY` attribute
- Public key is exported in uncompressed format (65 bytes: 0x04 || X || Y)
- **IMPORTANT**: Before running this example, you MUST uncomment `app_mbedtls_image3_init()` in the secure world boot file:
  - For RTL8721F: `component/soc/amebagreen2/img3/boot_img3.c`
  - Find the `BOOT_IMG3()` function and uncomment the line:
    ```c
    /* Initialize mbedtls for secure world, customers can enable it themselves as needed. */
    // app_mbedtls_image3_init();  // <-- Uncomment this line
    ```
  - After testing, you can comment it back to save resources

# Supported IC

RTL8721F
