#-------------------------------------------------------------------------------
# Copyright (c) 2020-2023, Arm Limited. All rights reserved.
# Copyright (c) 2021 STMicroelectronics. All rights reserved.
# Copyright (c) 2022 Cypress Semiconductor Corporation (an Infineon company)
# or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

########################## BL2 #################################################

set(MCUBOOT_IMAGE_NUMBER                2           CACHE STRING    "Whether to combine S and NS into either 1 image, or sign each seperately")
set(BL2_TRAILER_SIZE                    0x2000      CACHE STRING    "Trailer size")
set(MCUBOOT_ALIGN_VAL                   16          CACHE STRING    "Align option to build image with imgtool")
set(MCUBOOT_UPGRADE_STRATEGY            "SWAP_USING_SCRATCH"      CACHE STRING    "Upgrade strategy for images")
set(MCUBOOT_BOOTSTRAP                   ON          CACHE BOOL      "Allow initial state with images in secondary slots(empty primary slots)")
set(MCUBOOT_ENC_IMAGES                  ON          CACHE BOOL      "Enable encrypted image upgrade support")
set(MCUBOOT_ENCRYPT_RSA                 ON          CACHE BOOL      "Use RSA for encrypted image upgrade support")
################################## Dependencies ################################
set(TFM_PARTITION_INTERNAL_TRUSTED_STORAGE ON       CACHE BOOL      "Enable Internal Trusted Storage partition")
set(TFM_PARTITION_CRYPTO                ON          CACHE BOOL      "Enable Crypto partition")
set(CRYPTO_HW_ACCELERATOR               ON          CACHE BOOL      "Whether to enable the crypto hardware accelerator on supported platforms")
set(MBEDCRYPTO_BUILD_TYPE               minsizerel  CACHE STRING    "Build type of Mbed Crypto library")
set(TFM_DUMMY_PROVISIONING              OFF         CACHE BOOL      "Provision with dummy values. NOT to be used in production")
set(PLATFORM_DEFAULT_OTP_WRITEABLE      OFF         CACHE BOOL      "Use on chip flash with write support")
set(PLATFORM_DEFAULT_NV_COUNTERS        OFF         CACHE BOOL      "Use default nv counter implementation.")
set(PS_CRYPTO_AEAD_ALG                  PSA_ALG_GCM CACHE STRING    "The AEAD algorithm to use for authenticated encryption in Protected Storage")
set(MCUBOOT_FIH_PROFILE                 LOW         CACHE STRING    "Fault injection hardening profile [OFF, LOW, MEDIUM, HIGH]")

# Platform-specific configurations
set(CONFIG_TFM_USE_TRUSTZONE             ON)
set(TFM_MULTI_CORE_TOPOLOGY              OFF)
set(PLATFORM_HAS_FIRMWARE_UPDATE_SUPPORT ON)
set(STSAFEA                             ON          CACHE BOOL      "Activate ST SAFE SUPPORT")
set(MCUBOOT_DATA_SHARING                ON)
