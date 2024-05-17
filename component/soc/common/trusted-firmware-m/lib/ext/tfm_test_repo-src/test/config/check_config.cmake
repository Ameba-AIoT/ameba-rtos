#-------------------------------------------------------------------------------
# Copyright (c) 2021-2022, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

########################## Check Flags Input ###################################

tfm_invalid_config((NOT TFM_PARTITION_PROTECTED_STORAGE) AND (TEST_NS_PS OR TEST_S_PS))
tfm_invalid_config((NOT TFM_PARTITION_INTERNAL_TRUSTED_STORAGE) AND (TEST_NS_ITS OR TEST_S_ITS OR TEST_NS_PS))
tfm_invalid_config((NOT TFM_PARTITION_CRYPTO) AND (TEST_NS_CRYPTO OR TEST_S_CRYPTO))
tfm_invalid_config((NOT TFM_PARTITION_INITIAL_ATTESTATION) AND (TEST_NS_ATTESTATION OR TEST_S_ATTESTATION OR TEST_NS_QCBOR OR TEST_NS_T_COSE))
tfm_invalid_config((NOT TFM_PARTITION_PLATFORM) AND (TEST_NS_PLATFORM OR TEST_S_PLATFORM))
tfm_invalid_config(NOT TFM_PARTITION_FIRMWARE_UPDATE AND (TEST_NS_FWU OR TEST_S_FWU))

tfm_invalid_config((TEST_NS_IPC OR TEST_S_IPC) AND CONFIG_TFM_SPM_BACKEND_SFN)
tfm_invalid_config(TEST_S_SFN_BACKEND AND CONFIG_TFM_SPM_BACKEND_IPC)

tfm_invalid_config(CONFIG_TFM_FLOAT_ABI STREQUAL "soft" AND (TEST_S_FPU OR TEST_NS_FPU))
tfm_invalid_config((NOT TFM_MULTI_CORE_TOPOLOGY) AND TEST_NS_MULTI_CORE)
tfm_invalid_config(TEST_NS_T_COSE AND SYMMETRIC_INITIAL_ATTESTATION)
tfm_invalid_config((NOT TFM_NS_MANAGE_NSID) AND TEST_NS_MANAGE_NSID)

# So far all NS regression tests are triggered from NS app
tfm_invalid_config(NOT NS AND TEST_FRAMEWORK_NS)

########################## Check suites dependence #############################

tfm_invalid_config(TEST_NS_PS AND NOT TEST_NS_ITS)

############################ FLIH/SLIH dependence ##############################

# FLIH and SLIH testing can not be enabled at the same time
tfm_invalid_config(TEST_NS_SLIH_IRQ AND TEST_NS_FLIH_IRQ)
tfm_invalid_config(NOT PLATFORM_SLIH_IRQ_TEST_SUPPORT AND TEST_NS_SLIH_IRQ)
tfm_invalid_config(NOT PLATFORM_FLIH_IRQ_TEST_SUPPORT AND TEST_NS_FLIH_IRQ)
