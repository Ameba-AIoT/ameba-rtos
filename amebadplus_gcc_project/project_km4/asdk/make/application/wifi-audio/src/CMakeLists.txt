set(DIR ${BASEDIR}/component/application/wifi-audio)
set(WIFI_USRCFG_DIR ${DIR}/pal/amebadplus/usrcfg)

set(
    MODULE_IFLAGS
    ${BASEDIR}/component/audio/base/osal/osal_c/interfaces 
    ${BASEDIR}/component/audio/interfaces 
    ${DIR}/interfaces
)

set(MODULE_DEFINES __RTOS__)
set(MODULE_OPTIONS -Wno-multichar -Wno-unused-parameter -Wno-unused-const-variable -Werror)

set(
    CSRC 
    ${DIR}/cmd/audiofi_example.c 
)

set(
    CPPSRC 
    ${DIR}/cmd/audiofi_cmd.cpp 
    ${DIR}/cmd/audiofi_sink_cmd.cpp
)

set(
    SRC 
    ${CSRC}
    ${CPPSRC}
)


ameba_internal_library(wifi_audio_src)
target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${MODULE_DEFINES})
target_compile_options(${CURRENT_LIB_NAME} PRIVATE ${MODULE_OPTIONS})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${SRC})
