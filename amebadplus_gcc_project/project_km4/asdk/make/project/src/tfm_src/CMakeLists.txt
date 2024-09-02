set(DIR ${TFM_NS_DIR})
set(tfm_tests_dir ${CMAKE_BINARY_DIR}/trusted-firmware-m/tf-m-tests)

set(
    MODULE_IFLAGS
    ${DIR}/interface
    ${DIR}/interface/include
    ${DIR}/interface/include/crypto_keys
    ${FREERTOSDIR}/include
    ${FREERTOSDIR}/portable/GCC/AmebaDplus_KM4/non_secure
    ${FREERTOSDIR}/portable/GCC/AmebaDplus_KM4/secure
)

set(
    MODULE_C_OPTIONS
    -Wno-cast-function-type 
)

set(
    MODULE_C_DEFINES 
    PLATFORM_DEFAULT_CRYPTO_KEYS
    PLATFORM_NS_NV_COUNTERS=0
    PROJECT_CONFIG_HEADER_FILE=\"${DIR}/interface/config_profile_medium.h\" 
    PS_CRYPTO_AEAD_ALG_CCM
    PS_ENCRYPTION
    QCBOR_DISABLE_FLOAT_HW_USE
    QCBOR_DISABLE_PREFERRED_FLOAT
    TFM_PARTITION_CRYPTO
    TFM_PARTITION_IDLE
    TFM_PARTITION_INITIAL_ATTESTATION
    TFM_PARTITION_INTERNAL_TRUSTED_STORAGE
    TFM_PARTITION_LOG_LEVEL=TFM_PARTITION_LOG_LEVEL_SILENCE
    TFM_PARTITION_NS_AGENT_TZ
    TFM_PARTITION_PLATFORM
    TFM_PARTITION_PROTECTED_STORAGE
    TFM_PARTITION_TEST_CORE_IPC
    TFM_PARTITION_TEST_PS
    TFM_PARTITION_TEST_SECURE_SERVICES
)

if(CONFIG_TFM_TEST_ENABLE)
    list(
        APPEND MODULE_C_DEFINES
        DOMAIN_NS=1
        TEST_FRAMEWORK_NS
    )
endif()

file(COPY ${BASEDIR}/component/soc/common/trusted-firmware-m/interface DESTINATION ${DIR})

set(
    CSRC 
    ${DIR}/interface/src/tfm_psa_ns_api.c
    ${DIR}/interface/src/tfm_ps_api.c
    ${DIR}/interface/src/tfm_its_api.c
    ${DIR}/interface/src/tfm_crypto_api.c
    ${DIR}/interface/src/tfm_attest_api.c
    ${DIR}/interface/src/tfm_platform_api.c
    ${DIR}/interface/src/os_wrapper/tfm_ns_interface_rtos.c
    ${DIR}/ameba_freertos_wrapper.c
)

set(LINK_TFM_LIB)

if(CONFIG_TFM_TEST_ENABLE)
    list(find TFM_CMAKE_ARGU TEST_NS_ATTESTATION=OFF output result)
    if(NOT result EQUAL -1)
        list(
            APPEND LINK_TFM_LIB
            ${tfm_tests_dir}/test/secure_fw/suites/attestation/libtfm_test_suite_attestation_ns.a
            ${tfm_tests_dir}/test/secure_fw/suites/t_cose/libtfm_t_cose_ns.a
            ${tfm_tests_dir}/test/secure_fw/suites/qcbor/libtfm_qcbor_ns.a
        )
    endif()

    list(find TFM_CMAKE_ARGU TEST_NS_T_COSE=OFF output result)
    if(NOT result EQUAL -1)
        list(
            APPEND LINK_TFM_LIB
            ${tfm_tests_dir}/test/secure_fw/suites/t_cose/libtfm_test_suite_t_cose_ns.a
            ${tfm_tests_dir}/test/secure_fw/suites/t_cose/libtfm_t_cose_test.a
            ${tfm_tests_dir}/test/secure_fw/suites/t_cose/libtfm_t_cose_ns.a
            ${tfm_tests_dir}/test/secure_fw/suites/qcbor/libtfm_qcbor_ns.a
        )
    endif()

    list(find TFM_CMAKE_ARGU TEST_NS_CRYPTO=OFF output result)
    if(NOT result EQUAL -1)
        list(
            APPEND LINK_TFM_LIB
            ${tfm_tests_dir}/test/secure_fw/suites/crypto/libtfm_test_suite_crypto_ns.a
        )
    endif()

    list(find TFM_CMAKE_ARGU TEST_NS_ITS=OFF output result)
    if(NOT result EQUAL -1)
        list(
            APPEND LINK_TFM_LIB
            ${tfm_tests_dir}/test/secure_fw/suites/its/libtfm_test_suite_its_ns.a
        )
    endif()

    list(find TFM_CMAKE_ARGU TEST_NS_PS=OFF output result)
    if(NOT result EQUAL -1)
        list(
            APPEND LINK_TFM_LIB
            ${tfm_tests_dir}/test/secure_fw/suites/ps/libtfm_test_suite_ps_ns.a
        )
    endif()

    list(find TFM_CMAKE_ARGU TEST_NS_PLATFORM=OFF output result)
    if(NOT result EQUAL -1)
        list(
            APPEND LINK_TFM_LIB
            ${tfm_tests_dir}/test/secure_fw/suites/platform/libtfm_test_suite_platform_ns.a
        )
    endif()

    list(find TFM_CMAKE_ARGU TEST_NS_IPC=OFF output result)
    if(NOT result EQUAL -1)
        list(
            APPEND LINK_TFM_LIB
            ${tfm_tests_dir}/test/secure_fw/suites/spm/ipc/libtfm_test_suite_ipc_ns.a
        )
    endif()

    if(CONFIG_TFM_TEST_PSA_API)
        list(
            APPEND LINK_TFM_LIB
            ${tfm_tests_dir}/app/psa_api_tests/val/val_nspe.a
            ${tfm_tests_dir}/app/psa_api_tests/platform/pal_nspe.a
        )

        if(CONFIG_TEST_PSA_API_STORAGE)
            list(
                APPEND LINK_TFM_LIB
                ${tfm_tests_dir}/app/psa_api_tests/dev_apis/storage/test_combine.a
            )
        endif()

        if(CONFIG_TEST_PSA_API_CRYPTO)
            list(
                APPEND LINK_TFM_LIB
                ${tfm_tests_dir}/app/psa_api_tests/dev_apis/crypto/test_combine.a
            )
        endif()

        if(CONFIG_TEST_PSA_API_INITIAL_ATTESTATION)
            list(
                APPEND LINK_TFM_LIB
                ${tfm_tests_dir}/app/psa_api_tests/dev_apis/initial_attestation/test_combine.a
            )
        endif()
    endif()

    list(
        APPEND LINK_TFM_LIB
        ${tfm_tests_dir}/app/libtfm_test_app.a
        ${tfm_tests_dir}/app/libtfm_api_ns.a
    )

    list(
        APPEND CSRC 
        ${DIR}/ameba_tfm_test_cmd.c
    )
endif()

set(cmd)
foreach(libname ${LINK_TFM_LIB})
    list(
        APPEND cmd 
        COMMAND ${CMAKE_AR} x ${libname}
    )
endforeach()

ameba_library_common(tfm_interface_src)
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_DEFINES})
target_compile_options(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_OPTIONS})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})


file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/tfm_ns_obj)
add_custom_target(
    tfm_ns 
    ${cmd}
    COMMAND ${CMAKE_AR} x $<TARGET_FILE:${CURRENT_LIB_NAME}>
    COMMAND ${CMAKE_AR} rvs lib_tfm_ns.a *.o
    COMMAND ${CMAKE_COMMAND} -E copy lib_tfm_ns.a ${IMAGE_TARGET_FOLDER}/lib_tfm_ns.a
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tfm_ns_obj
    DEPENDS ${CURRENT_LIB_NAME}
)
add_dependencies(target_img2_${PROJECT_NAME} tfm_ns)
add_dependencies(tfm_ns target_tfm)