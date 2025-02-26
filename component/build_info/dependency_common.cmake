if (__INCLUDED_DEPENDENCY_COMMOM_CMAKE__)
    return()
else()
    set(__INCLUDED_DEPENDENCY_COMMOM_CMAKE__ TRUE)
endif()

#NOTE: Add in order of name
# ameba_target_add(dep_${d_MCU_PROJECT_NAME}_at_cmd p_TYPE interface)

# ameba_target_depend(dep_${d_MCU_PROJECT_NAME}_at_cmd ${d_CURRENT_TARGET_NAME})

add_library(dep_${d_MCU_PROJECT_NAME}_at_cmd INTERFACE)
add_library(dep_${d_MCU_PROJECT_NAME}_audio INTERFACE)
add_library(dep_${d_MCU_PROJECT_NAME}_benchmark INTERFACE)
add_library(dep_${d_MCU_PROJECT_NAME}_clintwood INTERFACE)
add_library(dep_${d_MCU_PROJECT_NAME}_demoui INTERFACE)            # In component/application/demoui
add_library(dep_${d_MCU_PROJECT_NAME}_example INTERFACE)
add_library(dep_${d_MCU_PROJECT_NAME}_emwin INTERFACE)
add_library(dep_${d_MCU_PROJECT_NAME}_file_system INTERFACE)
add_library(dep_${d_MCU_PROJECT_NAME}_flac INTERFACE)
add_library(dep_${d_MCU_PROJECT_NAME}_lwip INTERFACE)
add_library(dep_${d_MCU_PROJECT_NAME}_network INTERFACE)
add_library(dep_${d_MCU_PROJECT_NAME}_os INTERFACE)
add_library(dep_${d_MCU_PROJECT_NAME}_rt_xmodem INTERFACE)
add_library(dep_${d_MCU_PROJECT_NAME}_soc_common INTERFACE)
add_library(dep_${d_MCU_PROJECT_NAME}_ssl INTERFACE)
add_library(dep_${d_MCU_PROJECT_NAME}_stdlib INTERFACE)
add_library(dep_${d_MCU_PROJECT_NAME}_aivoice INTERFACE)
add_library(dep_${d_MCU_PROJECT_NAME}_tflite_micro INTERFACE)
add_library(dep_${d_MCU_PROJECT_NAME}_ui INTERFACE)                 # In component/ui
add_library(dep_${d_MCU_PROJECT_NAME}_usb INTERFACE)
add_library(dep_${d_MCU_PROJECT_NAME}_utilities INTERFACE)
add_library(dep_${d_MCU_PROJECT_NAME}_utilities_example INTERFACE)
add_library(dep_${d_MCU_PROJECT_NAME}_wifi_audio INTERFACE)
add_library(dep_${d_MCU_PROJECT_NAME}_wpan INTERFACE)

add_library(dep_${d_MCU_PROJECT_NAME}_jpeg INTERFACE)
add_library(dep_${d_MCU_PROJECT_NAME}_png INTERFACE)
add_library(dep_${d_MCU_PROJECT_NAME}_zlib INTERFACE)


include(${CMAKE_CURRENT_LIST_DIR}/at_cmd.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/audio.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/benchmark.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/clintwood.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/demoui.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/file_system.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/network.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/ssl.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/ui.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/usb.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/utilities.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/utilities_example.cmake)
if(NOT d_AMEBA_RELEASE)
    include(${CMAKE_CURRENT_LIST_DIR}/jpeg.cmake)
    include(${CMAKE_CURRENT_LIST_DIR}/png.cmake)
    include(${CMAKE_CURRENT_LIST_DIR}/wifi_audio.cmake)
    include(${CMAKE_CURRENT_LIST_DIR}/wpan.cmake)
    include(${CMAKE_CURRENT_LIST_DIR}/zlib.cmake)
endif()


###################################### dep_at_cmd begin #############################################
ameba_target_include(dep_${d_MCU_PROJECT_NAME}_at_cmd p_SCOPE interface ${c_COMPONENT_DIR}/at_cmd)
####################################### dep_at_cmd end ##############################################

######################################## dep_audio begin ##############################################
#NOTE: Here use list to remove DUPLICATES libs before link, which may cause link problem
set(_tmp_dep_${d_MCU_PROJECT_NAME}_audio_wt_whole_libs) # link with -Wl,--whole-archive
set(_tmp_dep_${d_MCU_PROJECT_NAME}_audio_wo_whole_libs) # link without -Wl,--whole-archive
if(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
    if("${d_MCU_PROJECT_NAME}" STREQUAL "hp")
        ameba_list_append_if(CONFIG_SPEEX_LIB _tmp_dep_${d_MCU_PROJECT_NAME}_audio_wo_whole_libs
            ${d_SDK_LIB_APPLICATION_DIR}/lib_speexdsp.a
        )
    elseif("${d_MCU_PROJECT_NAME}" STREQUAL "ap")
        ameba_list_append_if(CONFIG_WIFI_AUDIO _tmp_dep_${d_MCU_PROJECT_NAME}_audio_wo_whole_libs
            ${d_SDK_LIB_APPLICATION_DIR}/lib_wifi_audio.a
            ${d_SDK_LIB_APPLICATION_DIR}/lib_flac.a
        )
        ameba_list_append_if(CONFIG_SPEEX_LIB _tmp_dep_${d_MCU_PROJECT_NAME}_audio_wo_whole_libs
            ${d_SDK_LIB_APPLICATION_DIR}/lib_speexdsp.a # no-whole-archive
        )
        ameba_list_append_if(CONFIG_OPUS_LIB _tmp_dep_${d_MCU_PROJECT_NAME}_audio_wo_whole_libs
            ${d_SDK_LIB_APPLICATION_DIR}/lib_opus.a
        )
    endif()
endif()

if (CONFIG_MEDIA_PLAYER OR CONFIG_MEDIA_PLAYER_LITE) #FIXME: different between smart ap and hp
    ameba_list_append(_tmp_dep_${d_MCU_PROJECT_NAME}_audio_wo_whole_libs ${d_SDK_LIB_APPLICATION_DIR}/lib_playback.a)
    ameba_list_append_if(CONFIG_MEDIA_CODEC_AAC _tmp_dep_${d_MCU_PROJECT_NAME}_audio_wo_whole_libs
        ${d_SDK_LIB_APPLICATION_DIR}/lib_fdkaac.a
    )
    ameba_list_append_if(CONFIG_MEDIA_CODEC_HAAC _tmp_dep_${d_MCU_PROJECT_NAME}_audio_wo_whole_libs
        ${d_SDK_LIB_APPLICATION_DIR}/lib_haac.a
    )
    ameba_list_append_if(CONFIG_MEDIA_CODEC_OPUS _tmp_dep_${d_MCU_PROJECT_NAME}_audio_wo_whole_libs
        ${d_SDK_LIB_APPLICATION_DIR}/lib_opus.a
    )
    ameba_list_append_if(CONFIG_MEDIA_DEMUX_FLAC _tmp_dep_${d_MCU_PROJECT_NAME}_audio_wo_whole_libs
        ${d_SDK_LIB_APPLICATION_DIR}/lib_flac.a
    )
    ameba_list_append_if(CONFIG_MEDIA_DEMUX_OGG _tmp_dep_${d_MCU_PROJECT_NAME}_audio_wo_whole_libs
        ${d_SDK_LIB_APPLICATION_DIR}/lib_vorbisdec.a
    )
    ameba_list_append_if(CONFIG_MEDIA_CODEC_GSM _tmp_dep_${d_MCU_PROJECT_NAME}_audio_wo_whole_libs
        ${d_SDK_LIB_APPLICATION_DIR}/lib_gsm.a
    )
endif()

ameba_list_append_if(CONFIG_USB_HOST_EN _tmp_dep_${d_MCU_PROJECT_NAME}_audio_wt_whole_libs
    ${d_SDK_LIB_APPLICATION_DIR}/lib_usbh.a
)
ameba_list_append_if(CONFIG_USB_DEVICE_EN _tmp_dep_${d_MCU_PROJECT_NAME}_audio_wt_whole_libs
    ${d_SDK_LIB_APPLICATION_DIR}/lib_usbd.a
)
ameba_list_append_if(CONFIG_USB_DRD_EN _tmp_dep_${d_MCU_PROJECT_NAME}_audio_wt_whole_libs
    ${d_SDK_LIB_APPLICATION_DIR}/lib_usb.a
)

if(CONFIG_AUDIO_MIXER)
    ameba_list_append(_tmp_dep_${d_MCU_PROJECT_NAME}_audio_wo_whole_libs ${d_SDK_LIB_APPLICATION_DIR}/lib_audio_mixer.a)
elseif(CONFIG_AUDIO_PASSTHROUGH)
    ameba_list_append(_tmp_dep_${d_MCU_PROJECT_NAME}_audio_wo_whole_libs ${d_SDK_LIB_APPLICATION_DIR}/lib_audio_passthrough.a)
endif()

if (CONFIG_AUDIO_RPC_AFE OR CONFIG_AUDIO_LOCAL_AFE OR CONFIG_AUDIO_FWK)
    ameba_list_append(_tmp_dep_${d_MCU_PROJECT_NAME}_audio_wo_whole_libs ${d_SDK_LIB_APPLICATION_DIR}/lib_base.a)
    if(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
        if("${d_MCU_PROJECT_NAME}" STREQUAL "hp")
            ameba_list_append(_tmp_dep_${d_MCU_PROJECT_NAME}_audio_wo_whole_libs ${d_SDK_LIB_APPLICATION_DIR}/lib_cmsis_dsp.a)
        endif()
    endif()
endif()
list(REMOVE_DUPLICATES _tmp_dep_${d_MCU_PROJECT_NAME}_audio_wt_whole_libs)
list(REMOVE_DUPLICATES _tmp_dep_${d_MCU_PROJECT_NAME}_audio_wo_whole_libs)
if(_tmp_dep_${d_MCU_PROJECT_NAME}_audio_wt_whole_libs)
    ameba_target_link(dep_${d_MCU_PROJECT_NAME}_audio p_SCOPE interface p_WHOLE_ARCHIVE
        ${_tmp_dep_${d_MCU_PROJECT_NAME}_audio_wt_whole_libs}
    )
endif()
if(_tmp_dep_${d_MCU_PROJECT_NAME}_audio_wo_whole_libs)
    ameba_target_link(dep_${d_MCU_PROJECT_NAME}_audio p_SCOPE interface
        ${_tmp_dep_${d_MCU_PROJECT_NAME}_audio_wo_whole_libs}
    )
endif()
unset(_tmp_dep_${d_MCU_PROJECT_NAME}_audio_wt_whole_libs)
unset(_tmp_dep_${d_MCU_PROJECT_NAME}_audio_wo_whole_libs)
#--------------------------#
######################################### dep_audio end ###############################################

###################################### dep_benchmark begin ############################################

###################################### dep_benchmark end ##############################################


###################################### dep_clintwood begin #############################################
ameba_target_include(dep_${d_MCU_PROJECT_NAME}_clintwood p_SCOPE interface ${c_CMPT_APPLICATION_DIR}/clintwood/wifi)

#--------------------------#

#--------------------------#
####################################### dep_clintwood end ##############################################

###################################### dep_demoui begin #############################################

####################################### dep_demoui end ##############################################

###################################### dep_example begin #############################################
ameba_target_include(dep_${d_MCU_PROJECT_NAME}_example p_SCOPE interface ${c_COMPONENT_DIR}/example)
####################################### dep_example end ##############################################

###################################### dep_emwin begin #############################################
ameba_target_include(dep_${d_MCU_PROJECT_NAME}_emwin p_SCOPE interface ${c_COMPONENT_DIR}/ui/emWin/emwin/emWinLibrary/include)
####################################### dep_emwin end #############################################

###################################### dep_file_system begin #############################################
ameba_target_include(dep_${d_MCU_PROJECT_NAME}_file_system p_SCOPE interface
    ${c_CMPT_FILE_SYSTEM_DIR}/fatfs/${v_FATFS_VER}/include
    ${c_CMPT_FILE_SYSTEM_DIR}/fatfs
    ${c_CMPT_FILE_SYSTEM_DIR}/ftl
    ${c_CMPT_FILE_SYSTEM_DIR}/dct
    ${c_CMPT_FILE_SYSTEM_DIR}/littlefs
    ${c_CMPT_FILE_SYSTEM_DIR}/littlefs/${v_LITTLEFS_VER}
    ${c_CMPT_FILE_SYSTEM_DIR}/kv
    ${c_CMPT_FILE_SYSTEM_DIR}/vfs
)
####################################### dep_file_system end ##############################################

###################################### dep_flac begin #############################################
if(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
    if("${d_MCU_PROJECT_NAME}" STREQUAL "hp")
        ameba_target_link_if(CONFIG_AUDIO_EN dep_${d_MCU_PROJECT_NAME}_flac p_SCOPE interface
            ${d_SDK_LIB_APPLICATION_DIR}/lib_flac.a
        )
        if(CONFIG_MEDIA_PLAYER OR CONFIG_MEDIA_PLAYER_LITE)
            ameba_target_link(dep_${d_MCU_PROJECT_NAME}_flac p_SCOPE interface
                ${d_SDK_LIB_APPLICATION_DIR}/lib_flac.a
            )
        endif()
    endif()
endif()
####################################### dep_flac end #############################################

###################################### dep_network begin #############################################
ameba_target_include(dep_${d_MCU_PROJECT_NAME}_network p_SCOPE interface
    ${c_CMPT_NETWORK_DIR}
    ${c_CMPT_NETWORK_DIR}/rtsp
    ${c_CMPT_NETWORK_DIR}/coap/include
    ${c_CMPT_NETWORK_DIR}/mqtt/MQTTClient
    ${c_CMPT_NETWORK_DIR}/httplite
    ${c_CMPT_NETWORK_DIR}/xml
    ${c_CMPT_NETWORK_DIR}/cJSON
)
if(CONFIG_WLAN)
    if (CONFIG_AS_INIC_AP OR CONFIG_SINGLE_CORE_WIFI)
        ameba_target_link(dep_${d_MCU_PROJECT_NAME}_network p_SCOPE interface p_WHOLE_ARCHIVE
            ${d_SDK_LIB_APPLICATION_DIR}/lib_httpc.a
            ${d_SDK_LIB_APPLICATION_DIR}/lib_httpd.a
            ${d_SDK_LIB_APPLICATION_DIR}/lib_websocket.a
            # ${d_SDK_LIB_APPLICATION_DIR}/lib_coap.a
            # ${d_SDK_LIB_APPLICATION_DIR}/lib_eap.a
        )
        if(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
            if("${d_MCU_PROJECT_NAME}" STREQUAL "ap")
                ameba_target_link(dep_${d_MCU_PROJECT_NAME}_network p_SCOPE interface p_WHOLE_ARCHIVE
                    ${d_SDK_LIB_APPLICATION_DIR}/lib_ipnat.a
                    ${d_SDK_LIB_APPLICATION_DIR}/lib_ipdnrd.a
                )
            endif()
        endif()
    endif()
endif()
####################################### dep_network end ##############################################

###################################### dep_lwip begin ##############################################
ameba_target_include(dep_${d_MCU_PROJECT_NAME}_lwip p_SCOPE interface
    ${c_COMPONENT_DIR}/lwip/lwip_${v_LWIP_VER}/src/include
    # ${c_COMPONENT_DIR}/lwip/lwip_${v_LWIP_VER}/src/include/lwip
    ${c_COMPONENT_DIR}/lwip/lwip_${v_LWIP_VER}/src/include/lwip/apps
    ${c_COMPONENT_DIR}/lwip/lwip_${v_LWIP_VER}/src/include/netif
    # ${c_COMPONENT_DIR}/lwip/lwip_${v_LWIP_VER}/src/include/ipv4
    ${c_COMPONENT_DIR}/lwip/lwip_${v_LWIP_VER}/port/realtek
    ${c_COMPONENT_DIR}/lwip/lwip_${v_LWIP_VER}/port/realtek/freertos
    ${c_COMPONENT_DIR}/lwip/api
)
####################################### dep_lwip end ###############################################


###################################### dep_ui begin #############################################
ameba_target_include(dep_${d_MCU_PROJECT_NAME}_ui p_SCOPE interface
    ${c_COMPONENT_DIR}/ui/LVGL/lvgl/src
    ${c_COMPONENT_DIR}/ui/LVGL/lvgl/src/core
    ${c_COMPONENT_DIR}/ui/LVGL/lvgl/src/draw
    ${c_COMPONENT_DIR}/ui/LVGL/lvgl/src/extra
    ${c_COMPONENT_DIR}/ui/LVGL/lvgl/src/font
    ${c_COMPONENT_DIR}/ui/LVGL/lvgl/src/hal
    ${c_COMPONENT_DIR}/ui/LVGL/lvgl/src/misc
    ${c_COMPONENT_DIR}/ui/LVGL/lvgl/src/widgets
    ${c_COMPONENT_DIR}/ui/LVGL/port
    ${c_COMPONENT_DIR}/ui/LVGL/lv_drivers/interfaces
)
####################################### end ##############################################

###################################### dep_os begin #############################################
ameba_target_include(dep_${d_MCU_PROJECT_NAME}_os p_SCOPE interface
    ${c_CMPT_OS_DIR}/freertos
    ${c_CMPT_OS_DIR}/os_wrapper/include
)
####################################### dep_os end ##############################################

###################################### dep_soc_common begin #############################################
ameba_target_include(dep_${d_MCU_PROJECT_NAME}_soc_common p_SCOPE interface ${c_COMPONENT_DIR}/soc/common)
####################################### dep_soc_common end ##############################################

###################################### dep_ssl begin #############################################
ameba_target_include(dep_${d_MCU_PROJECT_NAME}_ssl p_SCOPE interface
    # ${c_COMPONENT_DIR}/ssl/ssl_ram_map/rom
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/include
    ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER}/library
    ${c_COMPONENT_DIR}/ssl/mbedtls_ram_map/rom
)

ameba_target_definitions(dep_${d_MCU_PROJECT_NAME}_ssl p_SCOPE interface
    MBEDTLS_CONFIG_FILE="mbedtls/config.h"
)
####################################### dep_ssl end #############################################


###################################### dep_stdlib begin #############################################
ameba_target_include(dep_${d_MCU_PROJECT_NAME}_stdlib p_SCOPE interface ${c_COMPONENT_DIR}/stdlib)
####################################### dep_stdlib end ###############################################


###################################### dep_tflite_micro begin #############################################
ameba_target_link_if(CONFIG_TFLITE_MICRO_EN dep_${d_MCU_PROJECT_NAME}_tflite_micro p_SCOPE interface
    ${d_SDK_LIB_APPLICATION_DIR}/lib_tflite_micro.a
)
####################################### dep_tflite_micro end ###############################################

###################################### dep_aivoice begin ###################################################
if(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
    if("${d_MCU_PROJECT_NAME}" STREQUAL "ap")

        set(AFE_RESOURCE afe_res_2mic50mm)
        if(CONFIG_AFE_RES_1MIC)
            set(AFE_RESOURCE afe_res_1mic)
        elseif(CONFIG_AFE_RES_2MIC30MM)
            set(AFE_RESOURCE afe_res_2mic30mm)
        elseif(CONFIG_AFE_RES_2MIC50MM)
            set(AFE_RESOURCE afe_res_2mic50mm)
        elseif(CONFIG_AFE_RES_2MIC70MM)
            set(AFE_RESOURCE afe_res_2mic70mm)
        endif()

        ameba_target_link_if(CONFIG_AIVOICE_EN dep_${d_MCU_PROJECT_NAME}_aivoice p_SCOPE interface
            ${c_COMPONENT_DIR}/aivoice/prebuilts/lib/rtl8730e_ca32_freertos/libaivoice.a
            ${c_COMPONENT_DIR}/aivoice/prebuilts/lib/rtl8730e_ca32_freertos/libafe_kernel.a
            ${c_COMPONENT_DIR}/aivoice/prebuilts/lib/rtl8730e_ca32_freertos/lib${AFE_RESOURCE}.a
            ${c_COMPONENT_DIR}/aivoice/prebuilts/lib/rtl8730e_ca32_freertos/libkernel.a
            ${c_COMPONENT_DIR}/aivoice/prebuilts/lib/rtl8730e_ca32_freertos/libfst.a
            ${c_COMPONENT_DIR}/aivoice/prebuilts/lib/rtl8730e_ca32_freertos/libkws.a
            ${c_COMPONENT_DIR}/aivoice/prebuilts/lib/rtl8730e_ca32_freertos/libasr.a
            ${c_COMPONENT_DIR}/aivoice/prebuilts/lib/rtl8730e_ca32_freertos/libvad.a
            ${c_COMPONENT_DIR}/aivoice/prebuilts/lib/rtl8730e_ca32_freertos/libtomlc99.a
            ${c_COMPONENT_DIR}/aivoice/prebuilts/lib/rtl8730e_ca32_freertos/libNE10.a
        )
    endif()
endif()
####################################### dep_aivoice end ####################################################

###################################### dep_usb begin #############################################
ameba_target_include(dep_${d_MCU_PROJECT_NAME}_usb p_SCOPE interface ${c_COMPONENT_DIR}/usb/host/cdc_ecm)
####################################### dep_usb end ##############################################

###################################### dep_utilities begin #############################################
if(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
    ameba_target_include(dep_${d_MCU_PROJECT_NAME}_utilities p_SCOPE interface
        ${c_CMPT_NETWORK_DIR}/httplite
        ${c_CMPT_NETWORK_DIR}/xml
        ${c_CMPT_NETWORK_DIR}/cJSON
        ${c_CMPT_NETWORK_DIR}/iperf
    )
endif()
####################################### dep_utilities end ##############################################


###################################### dep_utilities_example begin #############################################

####################################### dep_utilities_example end ##############################################

###################################### dep_wifi_audio begin ############################################
if(${d_PLATFORM_TYPE} STREQUAL "amebadplus")
    ameba_target_link_if(CONFIG_IMQ_EN dep_${d_MCU_PROJECT_NAME}_wifi_audio p_SCOPE interface
        -Wl,--whole-archive
        ${c_CMPT_APPLICATION_DIR}/wifi-audio/lib/lib_wfa_pal.a
        -Wl,--no-whole-archive
        ${c_CMPT_APPLICATION_DIR}/wifi-audio/lib/lib_wifi_audio.a
    )
endif()
####################################### dep_wifi_audio end #############################################

###################################### dep_wpan begin ############################################
ameba_target_include_if(CONFIG_802154_THREAD_EN dep_${d_MCU_PROJECT_NAME}_wpan p_SCOPE interface
    ${c_COMPONENT_DIR}/wpan/openthread/openthread/include
    ${c_COMPONENT_DIR}/wpan/openthread/openthread/src/core
    ${c_COMPONENT_DIR}/wpan/openthread/config
    ${c_COMPONENT_DIR}/wpan/openthread/openthread/third_party/mbedtls
)
if(CONFIG_802154_THREAD_EN)
    ameba_target_link(dep_${d_MCU_PROJECT_NAME}_wpan p_SCOPE interface
        $<TARGET_FILE:openthread-platform-utils-static>
        $<TARGET_FILE:openthread-platform>
    )
    if(CONFIG_802154_THREAD_BORDER_ROUTER_EN OR CONFIG_802154_THREAD_FTD_EN)
        ameba_target_link(dep_${d_MCU_PROJECT_NAME}_wpan p_SCOPE interface p_WHOLE_ARCHIVE
            $<TARGET_FILE:openthread-cli-ftd>
            $<TARGET_FILE:openthread-ftd>
            $<TARGET_FILE:tcplp-ftd>
        )
    endif()
    ameba_target_link_if(CONFIG_802154_THREAD_MTD_EN dep_${d_MCU_PROJECT_NAME}_wpan p_SCOPE interface
        $<TARGET_FILE:openthread-cli-mtd>
        $<TARGET_FILE:openthread-mtd>
        $<TARGET_FILE:tcplp-mtd>
    )
    ameba_target_link_if(CONFIG_802154_THREAD_RCP_EN dep_${d_MCU_PROJECT_NAME}_wpan p_SCOPE interface
        $<TARGET_FILE:openthread-cli-radio>
        $<TARGET_FILE:openthread-rcp>
        $<TARGET_FILE:openthread-radio-cli>
        $<TARGET_FILE:openthread-spinel-rcp>
        $<TARGET_FILE:openthread-hdlc>
    )
    if(CONFIG_802154_RADIO_EXT_RCP_RTK OR CONFIG_802154_RADIO_EXT_RCP_OTHER)
        ameba_target_link(dep_${d_MCU_PROJECT_NAME}_wpan p_SCOPE interface
            $<TARGET_FILE:openthread-hdlc>
            $<TARGET_FILE:openthread-radio-spinel>
            $<TARGET_FILE:openthread-spinel-rcp>
        )
    endif()
    ameba_target_link_if(CONFIG_802154_THREAD_BORDER_ROUTER_EN dep_${d_MCU_PROJECT_NAME}_wpan p_SCOPE interface
        ${d_SDK_LIB_APPLICATION_DIR}/lib_wpan_otbr.a
    )
endif()
if(CONFIG_802154_ZIGBEE_EN)
    ameba_target_link_if(CONFIG_802154_ZIGBEE_MAC_TEST_EN dep_${d_MCU_PROJECT_NAME}_wpan p_SCOPE interface
        ${d_SDK_LIB_APPLICATION_DIR}/lib_wpan_zb_zboss_mac_test.a
    )
if(CONFIG_802154_ZIGBEE_ZCP_TEST_EN)
if(CONFIG_802154_ZIGBEE_ZCP_DEFAULT_EN)
    ameba_target_link_if(CONFIG_802154_ZIGBEE_ROLE_ZC dep_${d_MCU_PROJECT_NAME}_wpan p_SCOPE interface
        ${d_SDK_LIB_APPLICATION_DIR}/lib_wpan_zb_zboss_zcp_test_default_zc_zr.a
    )
    ameba_target_link_if(CONFIG_802154_ZIGBEE_ROLE_ZR dep_${d_MCU_PROJECT_NAME}_wpan p_SCOPE interface
        ${d_SDK_LIB_APPLICATION_DIR}/lib_wpan_zb_zboss_zcp_test_default_zc_zr.a
    )
    ameba_target_link_if(CONFIG_802154_ZIGBEE_ROLE_ZED dep_${d_MCU_PROJECT_NAME}_wpan p_SCOPE interface
        ${d_SDK_LIB_APPLICATION_DIR}/lib_wpan_zb_zboss_zcp_test_default_ed.a
    )
endif()
if(CONFIG_802154_ZIGBEE_ZCP_GPPB_EN)
    ameba_target_link_if(CONFIG_802154_ZIGBEE_ROLE_ZC dep_${d_MCU_PROJECT_NAME}_wpan p_SCOPE interface
        ${d_SDK_LIB_APPLICATION_DIR}/lib_wpan_zb_zboss_zcp_test_gppb_zc.a
    )
    ameba_target_link_if(CONFIG_802154_ZIGBEE_ROLE_ZR dep_${d_MCU_PROJECT_NAME}_wpan p_SCOPE interface
        ${d_SDK_LIB_APPLICATION_DIR}/lib_wpan_zb_zboss_zcp_test_gppb_zr.a
    )
endif()
endif()
if(CONFIG_802154_ZIGBEE_REGRESSION_TEST_EN)

endif()
if(CONFIG_802154_ZIGBEE_APPLICATION_EN)
    ameba_target_link_if(CONFIG_802154_ZIGBEE_ROLE_ZC dep_${d_MCU_PROJECT_NAME}_wpan p_SCOPE interface
        ${d_SDK_LIB_APPLICATION_DIR}/lib_wpan_zb_zboss_zc_zr.a
    )
    ameba_target_link_if(CONFIG_802154_ZIGBEE_ROLE_ZR dep_${d_MCU_PROJECT_NAME}_wpan p_SCOPE interface
        ${d_SDK_LIB_APPLICATION_DIR}/lib_wpan_zb_zboss_zc_zr.a
    )
    ameba_target_link_if(CONFIG_802154_ZIGBEE_ROLE_ZED dep_${d_MCU_PROJECT_NAME}_wpan p_SCOPE interface
        ${d_SDK_LIB_APPLICATION_DIR}/lib_wpan_zb_zboss_ed.a
    )
endif()
endif()
if(CONFIG_802154_THREAD_EN OR CONFIG_802154_ZIGBEE_EN)
    ameba_target_link_if(CONFIG_802154_RADIO_INT_SW dep_${d_MCU_PROJECT_NAME}_wpan p_SCOPE interface
        ${d_SDK_LIB_APPLICATION_DIR}/lib_wpan_platform_sw_radio.a
    )
endif()
####################################### dep_wpan end #############################################

###################################### dep_jpeg begin #############################################
ameba_target_include(dep_${d_MCU_PROJECT_NAME}_jpeg p_SCOPE interface
    ${c_COMPONENT_DIR}/ui/third_party/libjpeg-turbo/interfaces/include
)
ameba_target_link(dep_${d_MCU_PROJECT_NAME}_jpeg p_SCOPE interface
    ${d_SDK_LIB_APPLICATION_DIR}/lib_jpeg.a
)
####################################### dep_jpeg end ##############################################

###################################### dep_png begin #############################################
ameba_target_include(dep_${d_MCU_PROJECT_NAME}_png p_SCOPE interface
    ${c_COMPONENT_DIR}/ui/third_party/libpng/interfaces/include
)
ameba_target_link(dep_${d_MCU_PROJECT_NAME}_png p_SCOPE interface
    ${d_SDK_LIB_APPLICATION_DIR}/lib_png.a
)
####################################### dep_png end ##############################################

###################################### dep_zlib begin #############################################
ameba_target_include(dep_${d_MCU_PROJECT_NAME}_zlib p_SCOPE interface
    ${c_COMPONENT_DIR}/ui/third_party/zlib/interfaces/include
)
ameba_target_link(dep_${d_MCU_PROJECT_NAME}_zlib p_SCOPE interface
    ${d_SDK_LIB_APPLICATION_DIR}/lib_zlib.a
)
####################################### dep_zlib end ##############################################
