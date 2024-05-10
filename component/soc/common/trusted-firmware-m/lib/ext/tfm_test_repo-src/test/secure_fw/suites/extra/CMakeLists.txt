#-------------------------------------------------------------------------------
# Copyright (c) 2021-2022, Arm Limited. All rights reserved.
# Copyright (c) 2022 Cypress Semiconductor Corporation (an Infineon company)
# or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_policy(SET CMP0079 NEW)

if (NOT EXTRA_NS_TEST_SUITE_PATH AND NOT EXTRA_S_TEST_SUITE_PATH)
    return()
endif()

####################### Non-Secure extra test library ##########################

if (EXTRA_NS_TEST_SUITE_PATH)
    add_library(tfm_test_suite_extra_ns STATIC EXCLUDE_FROM_ALL)

    target_include_directories(tfm_test_suite_extra_ns
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/non_secure
    )

    target_compile_definitions(tfm_test_suite_extra_ns
        INTERFACE
            EXTRA_NS_TEST_SUITE
    )

    target_link_libraries(tfm_test_suite_extra_ns
        PRIVATE
            tfm_test_framework_ns
    )

    target_link_libraries(tfm_ns_tests
        INTERFACE
            tfm_test_suite_extra_ns
    )
endif()

####################### Secure extra test library ##############################

if (EXTRA_S_TEST_SUITE_PATH)
    target_include_directories(tfm_test_suite_extra_s
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/secure
    )

    target_compile_definitions(tfm_test_suite_extra_s
        INTERFACE
            EXTRA_S_TEST_SUITE
    )

    target_link_libraries(tfm_test_suite_extra_s
        PRIVATE
            tfm_test_framework_s
            platform_s
    )

    target_link_libraries(tfm_s_tests
        INTERFACE
            tfm_test_suite_extra_s
    )
endif()

# Add extra test suites source from outside
include(extra_tests_config.cmake)
