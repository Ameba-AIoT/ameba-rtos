if (__INCLUDED_CLINTWOOD_BUILD_CMAKE__)
    return()
else()
    set(__INCLUDED_CLINTWOOD_BUILD_CMAKE__ TRUE)
endif()

add_library(src_${d_MCU_PROJECT_NAME}_clintwood_audio INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_clintwood_soc INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_clintwood_wifi INTERFACE)

#--------------------------#
ameba_target_include(src_${d_MCU_PROJECT_NAME}_clintwood_audio p_SCOPE interface
    # ${c_COMPONENT_DIR}/utils/ipc/ipc/amebasmart/include/include #FIXME: depends on platform
    # ${c_COMPONENT_DIR}/utils/ipc/ipc/amebasmart/include/src
    ${c_COMPONENT_DIR}/audio/interfaces
    ${c_COMPONENT_DIR}/audio/base/libs/src/include
    ${c_COMPONENT_DIR}/audio/base/log/interfaces
    ${c_COMPONENT_DIR}/audio/base/osal/osal_c/interfaces
    ${c_COMPONENT_DIR}/audio/base/osal/osal_cxx/interfaces
    ${c_COMPONENT_DIR}/audio/base/cutils/interfaces
    ${c_COMPONENT_DIR}/audio/base/utils/interfaces
    ${c_COMPONENT_DIR}/audio/base/cutils/interfaces
)
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_clintwood_audio p_SCOPE interface
    ${c_CMPT_APPLICATION_DIR}/clintwood/audio/msft_client/app_example.c
    ${c_CMPT_APPLICATION_DIR}/clintwood/audio/msft_client/audio_opus_encode_decode.c
    ${c_CMPT_APPLICATION_DIR}/clintwood/audio/msft_client/audio_record_play.c
    ${c_CMPT_APPLICATION_DIR}/clintwood/audio/msft_client/example_opus_rpc.c
    ${c_CMPT_APPLICATION_DIR}/clintwood/audio/msft_client/opus_rpc.c
    ${c_CMPT_AUDIO_DIR}/base/cutils/src/rtringbuffer.c
)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_clintwood_audio p_SCOPE interface RPC_SERVER)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_clintwood_soc p_SCOPE interface
    ${c_CMPT_APPLICATION_DIR}/clintwood/soc/user_heap.c
)
#--------------------------#
if(CONFIG_AMEBASMART)
    if("${d_MCU_PROJECT_NAME}" STREQUAL "hp")
        ameba_target_sources(src_${d_MCU_PROJECT_NAME}_clintwood_wifi p_SCOPE interface
            ${c_CMPT_APPLICATION_DIR}/clintwood/wifi/msft_wifi_twt.c
            ${c_CMPT_APPLICATION_DIR}/clintwood/wifi/msft_wifi_twt_softap.c
        )
    elseif("${d_MCU_PROJECT_NAME}" STREQUAL "lp")
        ameba_target_sources(src_${d_MCU_PROJECT_NAME}_clintwood_wifi p_SCOPE interface
            ${c_CMPT_APPLICATION_DIR}/clintwood/wifi/msft_wifi_twt.c
        )
    endif()
endif()
#--------------------------#
