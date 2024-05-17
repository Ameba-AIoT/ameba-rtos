#-------------------------------------------------------------------------------
# Copyright (c) 2020-2022, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_policy(SET CMP0079 NEW)

if (NOT TEST_NS_CRYPTO AND NOT TEST_S_CRYPTO)
    return()
endif()

####################### Non Secure #############################################

if (TEST_NS_CRYPTO)
    add_library(tfm_test_suite_crypto_ns STATIC EXCLUDE_FROM_ALL)

    target_sources(tfm_test_suite_crypto_ns
        PRIVATE
            ./crypto_tests_common.c
            ./non_secure/crypto_ns_interface_testsuite.c
    )

    target_include_directories(tfm_test_suite_crypto_ns
        PUBLIC
            ./non_secure
    )

    target_link_libraries(tfm_test_suite_crypto_ns
        PRIVATE
            tfm_config
            tfm_test_framework_ns
    )

    target_link_libraries(tfm_ns_tests
        INTERFACE
            tfm_test_suite_crypto_ns
    )

    target_compile_definitions(tfm_test_suite_crypto_ns
        PRIVATE
            DOMAIN_NS=1
            $<$<BOOL:${TFM_CRYPTO_TEST_ALG_CBC}>:TFM_CRYPTO_TEST_ALG_CBC>
            $<$<BOOL:${TFM_CRYPTO_TEST_ALG_CCM}>:TFM_CRYPTO_TEST_ALG_CCM>
            $<$<BOOL:${TFM_CRYPTO_TEST_ALG_CFB}>:TFM_CRYPTO_TEST_ALG_CFB>
            $<$<BOOL:${TFM_CRYPTO_TEST_ALG_ECB}>:TFM_CRYPTO_TEST_ALG_ECB>
            $<$<BOOL:${TFM_CRYPTO_TEST_ALG_CTR}>:TFM_CRYPTO_TEST_ALG_CTR>
            $<$<BOOL:${TFM_CRYPTO_TEST_ALG_OFB}>:TFM_CRYPTO_TEST_ALG_OFB>
            $<$<BOOL:${TFM_CRYPTO_TEST_ALG_GCM}>:TFM_CRYPTO_TEST_ALG_GCM>
            $<$<AND:$<BOOL:${TFM_CRYPTO_TEST_ALG_SHA_384}>,$<BOOL:${CC312_LEGACY_DRIVER_API_ENABLED}>>:TFM_CRYPTO_TEST_ALG_SHA_384>
            $<$<AND:$<BOOL:${TFM_CRYPTO_TEST_ALG_SHA_512}>,$<BOOL:${CC312_LEGACY_DRIVER_API_ENABLED}>>:TFM_CRYPTO_TEST_ALG_SHA_512>
            $<$<BOOL:${TFM_CRYPTO_TEST_HKDF}>:TFM_CRYPTO_TEST_HKDF>
            $<$<BOOL:${TFM_CRYPTO_TEST_ECDH}>:TFM_CRYPTO_TEST_ECDH>
            $<$<BOOL:${TFM_CRYPTO_TEST_CHACHA20}>:TFM_CRYPTO_TEST_CHACHA20>
            $<$<BOOL:${TFM_CRYPTO_TEST_ALG_CHACHA20_POLY1305}>:TFM_CRYPTO_TEST_ALG_CHACHA20_POLY1305>
            $<$<BOOL:${TFM_CRYPTO_TEST_ALG_RSASSA_PSS_VERIFICATION}>:TFM_CRYPTO_TEST_ALG_RSASSA_PSS_VERIFICATION>
            $<$<BOOL:${TFM_CRYPTO_TEST_SINGLE_PART_FUNCS}>:TFM_CRYPTO_TEST_SINGLE_PART_FUNCS>
            $<$<BOOL:${TFM_MBEDCRYPTO_PSA_CRYPTO_CONFIG_PATH}>:MBEDTLS_PSA_CRYPTO_CONFIG_FILE="${TFM_MBEDCRYPTO_PSA_CRYPTO_CONFIG_PATH}">
        INTERFACE
            TEST_NS_CRYPTO
    )
endif()

####################### Secure #################################################

if (TEST_S_CRYPTO)
    target_sources(tfm_test_suite_crypto_s
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/crypto_tests_common.c
            ${CMAKE_CURRENT_SOURCE_DIR}/secure/crypto_sec_interface_testsuite.c
    )

    target_include_directories(tfm_test_suite_crypto_s
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/secure
    )

    target_link_libraries(tfm_test_suite_crypto_s
        PRIVATE
            tfm_config
            tfm_test_framework_s
            tfm_app_rot_partition_secure_client_2
            platform_s
    )

    target_link_libraries(tfm_s_tests
        INTERFACE
            tfm_test_suite_crypto_s
    )

    target_compile_definitions(tfm_test_suite_crypto_s
        PRIVATE
            $<$<BOOL:${TFM_CRYPTO_TEST_ALG_CBC}>:TFM_CRYPTO_TEST_ALG_CBC>
            $<$<BOOL:${TFM_CRYPTO_TEST_ALG_CCM}>:TFM_CRYPTO_TEST_ALG_CCM>
            $<$<BOOL:${TFM_CRYPTO_TEST_ALG_CFB}>:TFM_CRYPTO_TEST_ALG_CFB>
            $<$<BOOL:${TFM_CRYPTO_TEST_ALG_ECB}>:TFM_CRYPTO_TEST_ALG_ECB>
            $<$<BOOL:${TFM_CRYPTO_TEST_ALG_CTR}>:TFM_CRYPTO_TEST_ALG_CTR>
            $<$<BOOL:${TFM_CRYPTO_TEST_ALG_OFB}>:TFM_CRYPTO_TEST_ALG_OFB>
            $<$<BOOL:${TFM_CRYPTO_TEST_ALG_GCM}>:TFM_CRYPTO_TEST_ALG_GCM>
            $<$<AND:$<BOOL:${TFM_CRYPTO_TEST_ALG_SHA_384}>,$<BOOL:${CC312_LEGACY_DRIVER_API_ENABLED}>>:TFM_CRYPTO_TEST_ALG_SHA_384>
            $<$<AND:$<BOOL:${TFM_CRYPTO_TEST_ALG_SHA_512}>,$<BOOL:${CC312_LEGACY_DRIVER_API_ENABLED}>>:TFM_CRYPTO_TEST_ALG_SHA_512>
            $<$<BOOL:${TFM_CRYPTO_TEST_HKDF}>:TFM_CRYPTO_TEST_HKDF>
            $<$<BOOL:${TFM_CRYPTO_TEST_ECDH}>:TFM_CRYPTO_TEST_ECDH>
            $<$<BOOL:${TFM_CRYPTO_TEST_CHACHA20}>:TFM_CRYPTO_TEST_CHACHA20>
            $<$<BOOL:${TFM_CRYPTO_TEST_ALG_CHACHA20_POLY1305}>:TFM_CRYPTO_TEST_ALG_CHACHA20_POLY1305>
            $<$<BOOL:${TFM_CRYPTO_TEST_ALG_RSASSA_PSS_VERIFICATION}>:TFM_CRYPTO_TEST_ALG_RSASSA_PSS_VERIFICATION>
            $<$<BOOL:${TFM_MBEDCRYPTO_PSA_CRYPTO_CONFIG_PATH}>:MBEDTLS_PSA_CRYPTO_CONFIG_FILE="${TFM_MBEDCRYPTO_PSA_CRYPTO_CONFIG_PATH}">
        INTERFACE
            TEST_S_CRYPTO
    )
endif()
