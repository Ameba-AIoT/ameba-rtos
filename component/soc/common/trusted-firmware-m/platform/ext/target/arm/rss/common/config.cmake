#-------------------------------------------------------------------------------
# Copyright (c) 2022-2023, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

if (RSS_XIP)
set(PLATFORM_DEFAULT_IMAGE_SIGNING      OFF       CACHE BOOL    "Use default image signing implementation")

set(MCUBOOT_S_IMAGE_FLASH_AREA_NUM      10        CACHE STRING  "ID of the flash area containing the primary Secure image")
set(MCUBOOT_NS_IMAGE_FLASH_AREA_NUM     11        CACHE STRING  "ID of the flash area containing the primary Non-Secure image")
endif()

set(CRYPTO_HW_ACCELERATOR               ON         CACHE BOOL     "Whether to enable the crypto hardware accelerator on supported platforms")
set(PLATFORM_DEFAULT_OTP                OFF        CACHE BOOL     "Use trusted on-chip flash to implement OTP memory")
set(PLATFORM_DEFAULT_CRYPTO_KEYS        OFF        CACHE BOOL     "Use default crypto keys implementation.")
set(PLATFORM_DEFAULT_PROVISIONING       OFF        CACHE BOOL     "Use default provisioning implementation")
set(TFM_BL1_DEFAULT_PROVISIONING        OFF        CACHE BOOL     "Whether BL1_1 will use default provisioning")
# CFB mode is not supported by CC312
set(TFM_CRYPTO_TEST_ALG_CFB             OFF        CACHE BOOL     "Test CFB cryptography mode")
set(PLATFORM_DEFAULT_NV_COUNTERS        OFF        CACHE BOOL     "Use default nv counter implementation.")
set(PLATFORM_DEFAULT_ATTEST_HAL         OFF        CACHE BOOL     "Use default attest hal implementation.")
set(PLATFORM_DEFAULT_SYSTEM_RESET_HALT  OFF        CACHE BOOL     "Use default system reset/halt implementation")
set(PLATFORM_HAS_BOOT_DMA               ON         CACHE BOOL     "Enable dma support for memory transactions for bootloader")
set(PLATFORM_BOOT_DMA_MIN_SIZE_REQ      0x40       CACHE STRING   "Minimum transaction size (in bytes) required to enable dma support for bootloader")
set(PLATFORM_SVC_HANDLERS               ON         CACHE BOOL     "Platform supports custom SVC handlers")

set(BL1                                 ON         CACHE BOOL     "Whether to build BL1")
set(PLATFORM_DEFAULT_BL1                ON         CACHE STRING   "Whether to use default BL1 or platform-specific one")
set(TFM_BL1_SOFTWARE_CRYPTO             OFF        CACHE BOOL     "Whether BL1_1 will use software crypto")
set(TFM_BL1_MEMORY_MAPPED_FLASH         ON         CACHE BOOL     "Whether BL1 can directly access flash content")
set(TFM_BL1_PQ_CRYPTO                   ON         CACHE BOOL     "Enable LMS PQ crypto for BL2 verification. This is experimental and should not yet be used in production")

set(BL2                                 ON         CACHE BOOL     "Whether to build BL2")
set(MCUBOOT_UPGRADE_STRATEGY            "RAM_LOAD" CACHE STRING   "Upgrade strategy when multiple boot images are loaded [OVERWRITE_ONLY, SWAP, DIRECT_XIP, RAM_LOAD]")
set(MCUBOOT_IMAGE_NUMBER                4          CACHE STRING   "Number of images supported by MCUBoot")
set(DEFAULT_MCUBOOT_FLASH_MAP           OFF        CACHE BOOL     "Whether to use the default flash map defined by TF-M project")
set(MCUBOOT_S_IMAGE_FLASH_AREA_NUM      2          CACHE STRING   "ID of the flash area containing the primary Secure image")
set(MCUBOOT_NS_IMAGE_FLASH_AREA_NUM     3          CACHE STRING   "ID of the flash area containing the primary Non-Secure image")
set(CONFIG_TFM_BOOT_STORE_ENCODED_MEASUREMENTS OFF CACHE BOOL     "Enable storing of encoded measurements in boot.")

set(TFM_PARTITION_CRYPTO                ON         CACHE BOOL     "Enable Crypto partition")
set(TFM_PARTITION_INITIAL_ATTESTATION   ON         CACHE BOOL     "Enable Initial Attestation partition")
set(TFM_PARTITION_PROTECTED_STORAGE     OFF        CACHE BOOL     "Enable Protected Storage partition")
set(TFM_PARTITION_INTERNAL_TRUSTED_STORAGE OFF     CACHE BOOL     "Enable Internal Trusted Storage partition")
set(TFM_PARTITION_PLATFORM              ON         CACHE BOOL     "Enable Platform partition")
set(MEASURED_BOOT_HASH_ALG              PSA_ALG_SHA_256 CACHE STRING "Hash algorithm used by Measured boot services")
set(TFM_MBEDCRYPTO_PLATFORM_EXTRA_CONFIG_PATH ${CMAKE_CURRENT_LIST_DIR}/mbedtls_extra_config.h CACHE PATH "Config to append to standard Mbed Crypto config, used by platforms to cnfigure feature support")

set(TFM_EXTRAS_REPO_PATH                "DOWNLOAD" CACHE PATH    "Path to tf-m-extras repo (or DOWNLOAD to fetch automatically")
set(TFM_EXTRAS_REPO_VERSION             "b30bc20"  CACHE STRING  "The version of tf-m-extras to use")
set(TFM_EXTRAS_REPO_EXTRA_PARTITIONS    "measured_boot;delegated_attestation" CACHE STRING "List of extra secure partition directory name(s)")
# Below TFM_EXTRAS_REPO_EXTRA_MANIFEST_LIST path is relative to tf-m-extras repo
set(TFM_EXTRAS_REPO_EXTRA_MANIFEST_LIST "partitions/measured_boot/measured_boot_manifest_list.yaml;partitions/delegated_attestation/delegated_attestation_manifest_list.yaml" CACHE STRING "List of extra secure partition manifests")

set(TFM_PARTITION_DELEGATED_ATTESTATION ON         CACHE BOOL     "Enable Delegated Attestation partition")

set(ATTEST_KEY_BITS                     384        CACHE STRING   "The size of the initial attestation key in bits")

set(TFM_NS_NV_COUNTER_AMOUNT            3          CACHE STRING   "How many NS NV counters are enabled")

set(TFM_PLAT_SPECIFIC_MULTI_CORE_COMM   ON         CACHE BOOL     "Whether to use a platform specific inter-core communication instead of mailbox in dual-cpu topology")
set(TEST_NS_MULTI_CORE                  OFF        CACHE BOOL     "Whether to build NS regression multi-core tests")

configure_file(${CMAKE_CURRENT_LIST_DIR}/manifest/tfm_manifest_list.yaml ${CMAKE_BINARY_DIR}/tools/tfm_manifest_list.yaml)
set(TFM_MANIFEST_LIST                   ${CMAKE_BINARY_DIR}/tools/tfm_manifest_list.yaml CACHE FILEPATH "TF-M native Secure Partition manifests list file")

# Platform-specific configurations
set(CONFIG_TFM_USE_TRUSTZONE            OFF)
set(TFM_MULTI_CORE_TOPOLOGY             ON)
set(MCUBOOT_DATA_SHARING                ON)
set(TFM_PARTITION_MEASURED_BOOT         ON)
