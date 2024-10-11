set(DIR ${BASEDIR}/component/audio/third_party/speexdsp)

set(
    MODULE_IFLAGS
    ${DIR}/libspeexdsp
    ${DIR}/include
)

set(
    MODULE_C_DEFINES
    HAVE_CONFIG_H
    FIXED_POINT
)

set(
    MODULE_C_OPTIONS
    -Wno-error
)

set(
    CSRC 
    ${DIR}/libspeexdsp/buffer.c
    ${DIR}/libspeexdsp/fftwrap.c
    ${DIR}/libspeexdsp/filterbank.c
    ${DIR}/libspeexdsp/jitter.c
    ${DIR}/libspeexdsp/kiss_fft.c
    ${DIR}/libspeexdsp/kiss_fftr.c
    ${DIR}/libspeexdsp/mdf.c
    ${DIR}/libspeexdsp/preprocess.c
    ${DIR}/libspeexdsp/resample.c
    ${DIR}/libspeexdsp/scal.c
    ${DIR}/libspeexdsp/smallft.c
)

ameba_app_library(speexdsp)
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_DEFINES})
target_compile_options(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_OPTIONS})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
