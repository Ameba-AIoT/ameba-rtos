set(DIR ${BASEDIR}/component/audio/third_party/flac)

set(
    MODULE_IFLAGS
    ${DIR}
    ${DIR}/include
    ${DIR}/src/libFLAC/include
)

set(
    MODULE_C_DEFINES
    __RTOS__
    HAVE_CONFIG_H
    FLAC__INTEGER_ONLY_LIBRARY
    FLAC__HAS_OGG=0
)

set(
    MODULE_C_OPTIONS
    -Wno-undef
    -Wno-format
    -Wno-implicit-fallthrough
    -Wno-discarded-qualifiers
    -Wno-incompatible-pointer-types
    -Wno-error
)

set(
    CSRC
    ${DIR}/src/libFLAC/bitmath.c
    ${DIR}/src/libFLAC/bitreader.c
    ${DIR}/src/libFLAC/bitwriter.c
    ${DIR}/src/libFLAC/cpu.c
    ${DIR}/src/libFLAC/crc.c
    ${DIR}/src/libFLAC/fixed.c
    ${DIR}/src/libFLAC/float.c
    ${DIR}/src/libFLAC/format.c
    ${DIR}/src/libFLAC/lpc.c
    ${DIR}/src/libFLAC/memory.c
    ${DIR}/src/libFLAC/md5.c
    ${DIR}/src/libFLAC/stream_decoder.c
    ${DIR}/src/libFLAC/stream_encoder.c
    ${DIR}/src/libFLAC/stream_encoder_framing.c
    ${DIR}/src/libFLAC/window.c
    ${DIR}/src/libFLAC/metadata_object.c
)

ameba_app_library(flac)
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_DEFINES})
target_compile_options(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_OPTIONS})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})