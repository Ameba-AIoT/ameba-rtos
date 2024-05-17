#-------------------------------------------------------------------------------
# Copyright (c) 2022-2023, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

########################## FPU and MVE #########################################

tfm_invalid_config(NOT CMAKE_C_COMPILER_ID STREQUAL "GNU" AND (CONFIG_TFM_ENABLE_MVE OR CONFIG_TFM_ENABLE_MVE_FP))
tfm_invalid_config((NOT CMAKE_C_COMPILER_ID STREQUAL "GNU" AND NOT CMAKE_C_COMPILER_ID STREQUAL "ARMClang") AND CONFIG_TFM_ENABLE_FP)
tfm_invalid_config((NOT CONFIG_TFM_FP_ARCH) AND (CONFIG_TFM_ENABLE_FP OR CONFIG_TFM_ENABLE_MVE_FP))
tfm_invalid_config((CMAKE_C_COMPILER_ID STREQUAL "ARMClang") AND (NOT CONFIG_TFM_FP_ARCH_ASM) AND CONFIG_TFM_ENABLE_FP)
tfm_invalid_config((NOT CONFIG_TFM_ENABLE_FP AND NOT CONFIG_TFM_ENABLE_MVE AND NOT CONFIG_TFM_ENABLE_MVE_FP) AND CONFIG_TFM_LAZY_STACKING)
tfm_invalid_config((CONFIG_TFM_ENABLE_FP OR CONFIG_TFM_ENABLE_MVE OR CONFIG_TFM_ENABLE_MVE_FP) AND NOT CONFIG_TFM_ENABLE_CP10CP11)

###################### Check compiler for FP vulnerability #####################

# Check compiler with mitigation for the VLLDM instruction security vulnerability or not.
# For more information, please check https://developer.arm.com/support/arm-security-updates/vlldm-instruction-security-vulnerability.
if (CONFIG_TFM_FLOAT_ABI STREQUAL "hard")
    # Create test C file.
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/cvetest.c "int x;")
    # Compile with mitigation -mfix-cmse-cve-2021-35465.
    if (CMAKE_C_COMPILER_ID STREQUAL "ARMClang")
        execute_process (
            COMMAND ${CMAKE_C_COMPILER} --target=${CROSS_COMPILE} ${CMAKE_C_FLAGS} -mcmse -mfix-cmse-cve-2021-35465 -S ${CMAKE_CURRENT_BINARY_DIR}/cvetest.c -o ${CMAKE_CURRENT_BINARY_DIR}/cvetest.s
            RESULT_VARIABLE ret
            ERROR_VARIABLE err
        )
    else()
        execute_process (
            COMMAND ${CMAKE_C_COMPILER} -mfix-cmse-cve-2021-35465 -S ${CMAKE_CURRENT_BINARY_DIR}/cvetest.c -o ${CMAKE_CURRENT_BINARY_DIR}/cvetest.s
            RESULT_VARIABLE ret
            ERROR_VARIABLE err
        )
    endif()
    file(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/cvetest.c)
    # Check result
    if(NOT ret EQUAL 0)
        message(FATAL_ERROR "To enable FPU usage in SPE and NSPE both, please use the compiler with '-mfix-cmse-cve-2021-35465' support")
    else()
        file(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/cvetest.s)
    endif()
endif()
