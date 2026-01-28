ameba_internal_library(example_full_flow_offline)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_full_flow_offline.c
    app_example.c
)

if(CONFIG_AMEBASMART)
    target_sources(
        ${CURRENT_LIB_NAME} PRIVATE
        nhxq_dkkt_nhxq_bfyy_echo_3c.c
    )
elseif(CONFIG_AMEBADPLUS)
    target_sources(
        ${CURRENT_LIB_NAME} PRIVATE
        nhxq_dkkt_gbkt_noecho_1c.c
    )
endif()


target_include_directories(
    ${CURRENT_LIB_NAME} PRIVATE
    ${BASEDIR}/component/utils/ringbuffer
)

target_compile_options(
    ${CURRENT_LIB_NAME} PRIVATE
    -Wno-undef
    -Wno-unused-parameter
)

target_compile_definitions(
    ${CURRENT_LIB_NAME} PRIVATE
    TF_LITE_STATIC_MEMORY
)

if(CONFIG_AMEBASMART)
    target_compile_definitions(
        ${CURRENT_LIB_NAME} PRIVATE
        AIVOICE_TARGET_AMEBADPLUS=0
    )
elseif(CONFIG_AMEBADPLUS)
    target_compile_definitions(
        ${CURRENT_LIB_NAME} PRIVATE
        AIVOICE_TARGET_AMEBADPLUS=1
    )
endif()

