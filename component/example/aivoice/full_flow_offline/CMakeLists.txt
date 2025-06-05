ameba_internal_library(example_full_flow_offline)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_full_flow_offline.c
    app_example.c
    nhxq_dkkt_nhxq_bfyy_echo_3c.c
)

target_include_directories(
    ${CURRENT_LIB_NAME} PRIVATE

    ${BASEDIR}/component/tflite_micro
    ${BASEDIR}/component/tflite_micro/third_party/gemmlowp
    ${BASEDIR}/component/tflite_micro/third_party/flatbuffers/include

    ${BASEDIR}/component/aivoice/include
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

