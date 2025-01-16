ameba_internal_library(example_tflm_hello_world)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE

    example_tflm_hello_world.cc
    models/hello_world_int8_model_data.cc
    models/hello_world_float_model_data.cc
    app_example.c
)

target_include_directories(
    ${CURRENT_LIB_NAME} PRIVATE

    ${BASEDIR}/component/tflite_micro
    ${BASEDIR}/component/tflite_micro/third_party/gemmlowp
    ${BASEDIR}/component/tflite_micro/third_party/flatbuffers/include

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
