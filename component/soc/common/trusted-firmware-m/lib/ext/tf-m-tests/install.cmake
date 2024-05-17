#-------------------------------------------------------------------------------
# Copyright (c) 2023, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

if(NOT TFM_NS_REG_TEST)
    return()
endif()

set(INSTALL_TEST_DIR    ${TFM_INSTALL_PATH}/test)

if(TEST_NS_ATTESTATION)
    install(DIRECTORY   ${CMAKE_SOURCE_DIR}/secure_fw/partitions/initial_attestation
            DESTINATION ${INSTALL_TEST_DIR}
            FILES_MATCHING PATTERN "*.h")
    install(FILES       ${CMAKE_SOURCE_DIR}/secure_fw/spm/include/boot/tfm_boot_status.h
            DESTINATION ${INSTALL_TEST_DIR}/initial_attestation)
endif()
