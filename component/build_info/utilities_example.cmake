if (__INCLUDED_UTILITIES_EXAMPLE_BUILD_CMAKE__)
    return()
else()
    set(__INCLUDED_UTILITIES_EXAMPLE_BUILD_CMAKE__ TRUE)
endif()

add_library(src_${d_MCU_PROJECT_NAME}_utilities_example INTERFACE)

#--------------------------#
ameba_target_include(src_${d_MCU_PROJECT_NAME}_utilities_example p_SCOPE interface
    ${c_CMPT_NETWORK_DIR}/cJSON
    ${c_CMPT_NETWORK_DIR}/coap/include
    ${c_CMPT_NETWORK_DIR}/mqtt/MQTTClient
    ${c_CMPT_NETWORK_DIR}/httplite
    ${c_CMPT_NETWORK_DIR}/xml
    ${c_CMPT_BLUETOOTH_DIR}
    ${c_COMPONENT_DIR}/sdio/sd_host/inc
    ${c_CMPT_WIFI_DIR}/realmesh/include
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_mesh_provisioner_rtk_demo
    ${c_CMPT_BLUETOOTH_DIR}/example/bt_mesh_device_rtk_demo
)
if(${d_PLATFORM_TYPE} STREQUAL "amebasmart")
    if("${d_MCU_PROJECT_NAME}" STREQUAL "ap")
        ameba_target_include(src_${d_MCU_PROJECT_NAME}_utilities_example p_SCOPE interface
            ${c_FREERTOS_DIR}/include
            ${c_FREERTOS_DIR}/portable/GCC/ARM_CA7
        )
        ameba_target_include_if(CONFIG_MEDIA_PLAYER src_${d_MCU_PROJECT_NAME}_utilities_example p_SCOPE interface
            ${c_CMPT_EXAMPLE_DIR}/player
            ${c_CMPT_EXAMPLE_DIR}/mydata_source_player
            ${c_CMPT_EXAMPLE_DIR}/player_cache_test
            ${c_CMPT_AUDIO_DIR}/interfaces
            ${c_CMPT_AUDIO_DIR}/base/libs/src/include
            ${c_CMPT_AUDIO_DIR}/base/log/interfaces
            ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/interfaces
            ${c_CMPT_AUDIO_DIR}/base/osal/osal_cxx/interfaces
            ${c_CMPT_AUDIO_DIR}/base/cutils/interfaces
            ${c_CMPT_AUDIO_DIR}/base/utils/interfaces
        )
        ameba_target_include_if(CONFIG_AUDIO_FWK src_${d_MCU_PROJECT_NAME}_utilities_example p_SCOPE interface
            ${c_CMPT_AUDIO_DIR}/interfaces
            ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/interfaces
        )
        ameba_target_include_if(CONFIG_SPEEX_LIB src_${d_MCU_PROJECT_NAME}_utilities_example p_SCOPE interface
            ${c_CMPT_EXAMPLE_DIR}/audio/speexdsp
            ${c_CMPT_AUDIO_DIR}/third_party/speexdsp/include
        )
    elseif("${d_MCU_PROJECT_NAME}" STREQUAL "hp")
        ameba_target_include(src_${d_MCU_PROJECT_NAME}_utilities_example p_SCOPE interface
            ${c_FREERTOS_DIR}/portable/GCC/AmebaSmart_HP/non_secure
            ${c_FREERTOS_DIR}/portable/GCC/AmebaSmart_HP/secure
        )
        ameba_target_include_if(CONFIG_AUDIO_FWK src_${d_MCU_PROJECT_NAME}_utilities_example p_SCOPE interface
            ${c_CMPT_AUDIO_DIR}/interfaces
            ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/interfaces
            ${c_CMPT_EXAMPLE_DIR}/audio_track
            ${c_CMPT_EXAMPLE_DIR}/audio_record
        )
    endif()
endif()

# WARNING: if dep_${d_MCU_PROJECT_NAME}_usb includes these dirs below, remove below!
ameba_target_include_if(CONFIG_USB_DEVICE_EN src_${d_MCU_PROJECT_NAME}_utilities_example p_SCOPE interface
    ${c_CMPT_USB_DIR}/common
    ${c_CMPT_USB_DIR}/device/core
    ${c_CMPT_USB_DIR}/device/cdc_acm
    ${c_CMPT_USB_DIR}/device/composite
    ${c_CMPT_USB_DIR}/device/hid
    ${c_CMPT_USB_DIR}/device/msc
    ${c_CMPT_USB_DIR}/device/uac
    ${c_CMPT_USB_DIR}/device/vendor
)
ameba_target_include_if(CONFIG_USB_HOST_EN src_${d_MCU_PROJECT_NAME}_utilities_example p_SCOPE interface
    ${c_CMPT_USB_DIR}/common
    ${c_CMPT_USB_DIR}/host/core
    ${c_CMPT_USB_DIR}/host/cdc_acm
    ${c_CMPT_USB_DIR}/host/cdc_ecm
    ${c_CMPT_USB_DIR}/host/msc
    ${c_CMPT_USB_DIR}/host/vendor
    ${c_CMPT_USB_DIR}/host/uvc
    ${c_COMPONENT_DIR}/utils/ringbuffer
)
ameba_target_include_if(CONFIG_USB_DRD_EN src_${d_MCU_PROJECT_NAME}_utilities_example p_SCOPE interface
    ${c_CMPT_USB_DIR}/common
    ${c_CMPT_USB_DIR}/device/core
    ${c_CMPT_USB_DIR}/device/cdc_a
    ${c_CMPT_USB_DIR}/device/compo
    ${c_CMPT_USB_DIR}/device/hid
    ${c_CMPT_USB_DIR}/device/msc
    ${c_CMPT_USB_DIR}/device/vendo
    ${c_CMPT_USB_DIR}/host/core
    ${c_CMPT_USB_DIR}/host/cdc_acm
    ${c_CMPT_USB_DIR}/host/cdc_ecm
    ${c_CMPT_USB_DIR}/host/msc
    ${c_CMPT_USB_DIR}/host/vendor
    ${c_CMPT_USB_DIR}/host/uvc
    ${c_COMPONENT_DIR}/utils/ringbuffer
)

ameba_target_include_if(CONFIG_AUDIO_FWK src_${d_MCU_PROJECT_NAME}_utilities_example p_SCOPE interface
    ${c_COMPONENT_DIR}/example/audio_track
    ${c_COMPONENT_DIR}/example/audio_record
    ${c_CMPT_AUDIO_DIR}/interfaces
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/interfaces
)
ameba_target_include_if(CONFIG_MEDIA_PLAYER src_${d_MCU_PROJECT_NAME}_utilities_example p_SCOPE interface
    ${c_CMPT_EXAMPLE_DIR}/player
    ${c_CMPT_EXAMPLE_DIR}/mydata_source_player
    ${c_CMPT_EXAMPLE_DIR}/player_cache_test
    ${c_CMPT_AUDIO_DIR}/interfaces
    ${c_CMPT_AUDIO_DIR}/base/libs/src/include
    ${c_CMPT_AUDIO_DIR}/base/log/interfaces
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/interfaces
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_cxx/interfaces
    ${c_CMPT_AUDIO_DIR}/base/cutils/interfaces
    ${c_CMPT_AUDIO_DIR}/base/utils/interfaces
)
ameba_target_include_if(CONFIG_SPEEX_LIB src_${d_MCU_PROJECT_NAME}_utilities_example p_SCOPE interface
    ${c_CMPT_EXAMPLE_DIR}/audio/speexdsp
    ${c_CMPT_AUDIO_DIR}/third_party/speexdsp/include
)

if(CONFIG_TFLITE_MICRO_EN)
    ameba_target_include(src_${d_MCU_PROJECT_NAME}_utilities_example p_SCOPE interface
        ${c_COMPONENT_DIR}/tflite_micro
        ${c_COMPONENT_DIR}/tflite_micro/third_party/gemmlowp
        ${c_COMPONENT_DIR}/tflite_micro/third_party/flatbuffers/include
    )
    ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_utilities_example p_SCOPE interface
        -Wno-undef
        -Wno-unused-parameter
    )
    ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_utilities_example p_SCOPE interface
        TF_LITE_STATIC_MEMORY
    )
endif()

if(CONFIG_AIVOICE_EN)
    ameba_target_include(src_${d_MCU_PROJECT_NAME}_utilities_example p_SCOPE interface
        ${c_COMPONENT_DIR}/aivoice/include
        ${c_COMPONENT_DIR}/utils/ringbuffer
    )
endif()

file(GLOB_RECURSE _tmp_EXAMPLE_DIR
    ${c_CMPT_EXAMPLE_DIR}/example_${EXAMPLE}.c
    ${c_CMPT_EXAMPLE_DIR}/example_${EXAMPLE}.cc
)
cmake_path(REMOVE_FILENAME _tmp_EXAMPLE_DIR)

file(GLOB_RECURSE _tmp_EXAMPLE_SRC
    ${_tmp_EXAMPLE_DIR}*.c
    ${_tmp_EXAMPLE_DIR}*.cc
)

ameba_target_include(src_${d_MCU_PROJECT_NAME}_utilities_example p_SCOPE interface
    ${_tmp_EXAMPLE_DIR}
)
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_utilities_example p_SCOPE interface
    ${_tmp_EXAMPLE_SRC}
)
unset(_tmp_EXAMPLE_DIR)
unset(_tmp_EXAMPLE_SRC)
#--------------------------#
