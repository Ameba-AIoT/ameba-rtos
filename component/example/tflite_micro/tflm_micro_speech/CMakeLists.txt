ameba_internal_library(example_tflm_micro_speech)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE

    testdata/no_1000ms_audio_data.cc
    testdata/yes_1000ms_audio_data.cc
    testdata/silence_1000ms_audio_data.cc
    testdata/no_30ms_audio_data.cc
    testdata/yes_30ms_audio_data.cc
    testdata/noise_1000ms_audio_data.cc
    example_tflm_micro_speech.cc
    models/audio_preprocessor_int8_model_data.cc
    models/micro_speech_quantized_model_data.cc

    app_example.c
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
