set(
    MODULE_IFLAGS
    ${BASEDIR}/component/audio/interfaces
    ${BASEDIR}/component/audio/base/cutils/interfaces
    ${BASEDIR}/component/audio/base/osal/osal_c/interfaces
    ${FREERTOSDIR}/include/
    ${FREERTOSDIR}/portable/GCC/AmebaDplus_KM4/non_secure
    ${FREERTOSDIR}/portable/GCC/AmebaDplus_KM4/secure
)

if(CONFIG_CLINTWOOD)
    list(APPEND MODULE_IFLAGS ${BASEDIR}/component/application/clintwood/soc)
endif()

set(DIR ${BASEDIR}/component/audio/audio_hal/amebadplus)

set(MODULE_C_DEFINES __RTOS__ NDEBUG)
set(MODULE_C_OPTIONS -UDEBUG -Wno-multichar -Werror)

if(CONFIG_WIFI_AUDIO)
    list(APPEND MODULE_C_DEFINES CONFIG_WL_AUDIO)
endif()

set(
    CSRC 
    ${DIR}/ameba_audio_stream_buffer.c
    ${DIR}/ameba_audio_stream.c
    ${DIR}/ameba_audio_stream_control.c
    ${DIR}/ameba_audio_stream_utils.c
    ${DIR}/ameba_audio_stream_render.c
    ${DIR}/ameba_audio_stream_capture.c
    ${DIR}/ameba_audio_stream_audio_patch.c
    ${DIR}/primary_audio_hw_stream_out.c
    ${DIR}/primary_audio_hw_card.c
    ${DIR}/primary_audio_hw_stream_in.c
    ${DIR}/audio_hw_params_handle.c
    ${DIR}/audio_hw_manager.c
    ${DIR}/audio_hw_control.c
)

ameba_internal_library(audio_hal)
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_DEFINES})
target_compile_options(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_OPTIONS})
