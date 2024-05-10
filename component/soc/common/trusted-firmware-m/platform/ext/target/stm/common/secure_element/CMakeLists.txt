#-------------------------------------------------------------------------------
# Copyright (c) 2020, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_policy(SET CMP0076 NEW)

set(STSAFEA_DIR ${CMAKE_CURRENT_LIST_DIR}/stsafea)
set(SE_PSA_DIR ${STSAFEA_DIR}/se_psa)

#========================= stsafea defs ===============================#

target_sources(crypto_service_mbedcrypto
    PRIVATE
        ${SE_PSA_DIR}/se_psa.c
        ${STSAFEA_INTERFACE_SRC_DIR}/stsafea_service_stub.c
)

target_include_directories(crypto_service_mbedcrypto
    PUBLIC
        ${SE_PSA_DIR}
        ${STSAFEA_INT_DIR}/Inc
)

target_compile_options(crypto_service_mbedcrypto
    PUBLIC
    -DPSA_USE_SE_ST
)

target_compile_options(tfm_psa_rot_partition_crypto
    PUBLIC
    -DPSA_USE_SE_ST
)

install(FILES  ${SE_PSA_DIR}/se_psa_id.h
    DESTINATION ${TFM_INSTALL_PATH}/interface/include)
