Mbed TLS 3.6.2 Porting Note

## 1. Time

Enable the MBEDTLS_HAVE_TIME and MBEDTLS_TIMING_ALT macros, and disable MBEDTLS_HAVE_TIME_DATE.
In this way, there is only relative time control, not absolute time (only affects X.509 certificate verification). 
And in the documentation it is recommended to use the sntp service to get the time.

## 2. mbedtls_config.h

For AmebaGreen2: 
In `mbedtls-3.6.2/include/mbedtls/build_info.h`, if not defined `MBEDTLS_CONFIG_FILE`, different config files will be included by default according to different compilation options. Some function macros and hardware acceleration macros were modified in `mbedtls_config_green2.h`.

For other plantform: if no extra modification in Makefile, just use the default mbedtls_config.h

```c
#if !defined(MBEDTLS_CONFIG_FILE)
    #include "ameba.h"
    
    #if defined(CONFIG_AMEBAGREEN2) && CONFIG_AMEBAGREEN2
        #if defined(CONFIG_BUILD_ROM)
            #include "mbedtls/mbedtls_config_green2_rom.h"
        #else
            #include "mbedtls/mbedtls_config_green2.h"
        #endif
    #else
        #include "mbedtls/mbedtls_config.h"
    #endif
#else
    #include MBEDTLS_CONFIG_FILE
#endif
```

## 3. Hardware Alternatives

For AmebaGreen2 / AmebaL2:

### 3.1 Entropy
Improved safety. We provide TRNG based implementation of a hardware entropy collector.

- MBEDTLS_ENTROPY_HARDWARE_ALT

source files: entropy_hardware_alt.c

### 3.2 AES / SHA
Accelerate calculation. Official replacement (whole module replacement). Use the following macros to enable hardware acceleration:

- MBEDTLS_AES_ALT (ECB in Slave mode, Others in DMA mode.)
- MBEDTLS_SHA256_ALT (DMA read only mode)
- MBEDTLS_SHA512_ALT (DMA read only mode)

source files: aes_alt.c, sha256_alt.c, sha512_alt.c

### 3.3 ECC
Accelerate calculation. Official replacement (single function replacement). Use the following macros to enable hardware acceleration:

- MBEDTLS_ECDH_GEN_PUBLIC_ALT
    - mbedtls_ecdh_gen_public
- MBEDTLS_ECDH_COMPUTE_SHARED_ALT
    - mbedtls_ecdh_compute_shared
- MBEDTLS_ECDSA_VERIFY_ALT
    - mbedtls_ecdsa_verify
- MBEDTLS_ECDSA_SIGN_ALT
    - mbedtls_ecdsa_sign
- MBEDTLS_ECDSA_GENKEY_ALT
    - mbedtls_ecdsa_genkey

source files: ecc.c

### 3.4 RSA
Accelerate calculation. The official does not provide single function replacement. (The official entire module replacement is too complicated and is not adopted).
We take an unofficial method to replace these functions. Use the following macros to enable hardware acceleration:

- RTK_LALU_MBEDTLS_RSA_CACULATION_ALT
    - mbedtls_rsa_private: write signature, encryption
    - mbedtls_rsa_public: read signature, decryption
    - mbedtls_rsa_gen_key: RSA key generation (HW engine only support 128-bit and above key)

source files: rsa_alt.c

## 4. ROM porting

For AmebaGreen2:
 
In order to put some global variables in rom bss, secure ROM compiles ./library_rom source files. 

ROM Enabled the following macros:

- MBEDTLS_ENTROPY_HARDWARE_ALT
- MBEDTLS_AES_ALT
- MBEDTLS_SHA256_ALT
- MBEDTLS_SHA512_ALT
- MBEDTLS_ECDH_GEN_PUBLIC_ALT
- MBEDTLS_ECDH_COMPUTE_SHARED_ALT
- MBEDTLS_ECDSA_VERIFY_ALT
- MBEDTLS_ECDSA_SIGN_ALT
- MBEDTLS_ECDSA_GENKEY_ALT

## 6. private_access.h
#define MBEDTLS_PRIVATE(member) member

## 7. Others
1. We provide a hardware-based EDDSA API (not Mbedtls related)
2. Modify net_sockets.c to support IPv4 only
3. timing.c, timing_alt.h use MBEDTLS_TIMING_ALT
