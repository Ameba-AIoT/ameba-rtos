ameba_internal_library(example_tflm_mnist)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE

    testdata/input_int8_test_data.cc
    testdata/label_int8_test_data.cc
    example_tflm_mnist.cc
    models/model_int8_tflm_optimized_model_data.cc

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
