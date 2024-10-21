set(DIR ${BASEDIR}/component/audio/third_party/tremolo)
set(MODULE_IFLAGS ${DIR})

set(
    MODULE_C_DEFINES
    __RTOS__
    ONLY_C
)
set(MODULE_C_OPTIONS -Wno-error)

set(
    CSRC
    ${DIR}/bitwise.c
    ${DIR}/codebook.c
    ${DIR}/dsp.c
    ${DIR}/floor0.c
    ${DIR}/floor1.c
    ${DIR}/floor_lookup.c
    ${DIR}/framing.c
    ${DIR}/mapping0.c
    ${DIR}/misc.c
    ${DIR}/res012.c
    ${DIR}/treminfo.c
    ${DIR}/vorbisfile.c
)

ameba_app_library(vorbisdec)


list(APPEND CSRC ${DIR}/mdct.c)


target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_DEFINES})
target_compile_options(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_OPTIONS})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
