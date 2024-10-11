set(DIR ${BASEDIR}/component/audio/configs)

set(
    MODULE_IFLAGS 
    ${DIR}/include
    ${BASEDIR}/component/audio/interfaces
)

set(
    MODULE_CPP_DEFINES
    __RTOS__
)

set(
    MODULE_CPP_OPTIONS
    -Wno-multichar -Wno-unused-parameter -std=gnu++17 -std=c++17 -Werror
)

set(CPPSRC ${DIR}/ameba_audio_mixer_usrcfg.cpp)



ameba_internal_library(audio_config)
target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${MODULE_CPP_DEFINES}>)
target_compile_options(${CURRENT_LIB_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${MODULE_CPP_OPTIONS}>)
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CPPSRC})

