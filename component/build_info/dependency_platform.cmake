if (__INCLUDED_DEPENDENCY_PLATFORM_CMAKE__)
    return()
else()
    set(__INCLUDED_DEPENDENCY_PLATFORM_CMAKE__ TRUE)
endif()


if (NOT d_PLATFORM_TYPE)
	ameba_fatal("Make sure ameba_platform_project_create and ameba_platform_project_init are called before include this cmake file")
endif()

# If necessary, define library based on ${d_PLATFORM_TYPE}, for example:
# if (${d_PLATFORM_TYPE} STREQUAL "amebadplus")
#     add_library(dep_${d_MCU_PROJECT_NAME}_demo_only_for_dplus INTERFACE)
# endif()


add_library(dep_${d_MCU_PROJECT_NAME}_app_monitor INTERFACE)
add_library(dep_${d_MCU_PROJECT_NAME}_app_xmodem INTERFACE)
add_library(dep_${d_MCU_PROJECT_NAME}_cmsis INTERFACE)
add_library(dep_${d_MCU_PROJECT_NAME}_cmsis_dsp INTERFACE)
add_library(dep_${d_MCU_PROJECT_NAME}_hal INTERFACE)
add_library(dep_${d_MCU_PROJECT_NAME}_misc INTERFACE)
add_library(dep_${d_MCU_PROJECT_NAME}_rtk_coex INTERFACE)
add_library(dep_${d_MCU_PROJECT_NAME}_rtk_coex_api INTERFACE)
add_library(dep_${d_MCU_PROJECT_NAME}_swlib INTERFACE)
add_library(dep_${d_MCU_PROJECT_NAME}_test INTERFACE)
add_library(dep_${d_MCU_PROJECT_NAME}_usrcfg INTERFACE)
add_library(dep_${d_MCU_PROJECT_NAME}_utils INTERFACE)
add_library(dep_${d_MCU_PROJECT_NAME}_wifi INTERFACE)

add_library(dep_${d_MCU_PROJECT_NAME}_img3 INTERFACE)

include(${CMAKE_CURRENT_LIST_DIR}/app_monitor.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmsis.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmsis_dsp.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/hal.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/misc.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/swlib.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/test.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/utils.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/wifi.cmake)

include(${CMAKE_CURRENT_LIST_DIR}/img3.cmake)

if(NOT d_AMEBA_RELEASE)
    include(${CMAKE_CURRENT_LIST_DIR}/rtk_coex.cmake)
    include(${CMAKE_CURRENT_LIST_DIR}/rtk_coex_api.cmake)
endif()

#################################### dep_app_monitor begin #############################################
ameba_target_include(dep_${d_MCU_PROJECT_NAME}_app_monitor p_SCOPE interface ${d_SOC_PLATFORM_DIR}/app/monitor/include)
##################################### dep_app_monitor end ##############################################


###################################### dep_app_xmodem begin ############################################
ameba_target_include(dep_${d_MCU_PROJECT_NAME}_app_xmodem p_SCOPE interface ${d_SOC_PLATFORM_DIR}/app/xmodem)
####################################### dep_app_xmodem end #############################################

###################################### dep_cmsis begin #############################################
ameba_target_include(dep_${d_MCU_PROJECT_NAME}_cmsis p_SCOPE interface ${d_SOC_PLATFORM_DIR}/cmsis)
####################################### dep_cmsis end ##############################################

#################################### dep_cmsis_dsp begin ###########################################
ameba_target_include(dep_${d_MCU_PROJECT_NAME}_cmsis_dsp p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/cmsis-dsp/Include
    ${d_SOC_PLATFORM_DIR}/cmsis-dsp/PrivateInclude
)
##################################### dep_cmsis_dsp end ############################################

####################################### dep_hal begin #############################################
ameba_target_include(dep_${d_MCU_PROJECT_NAME}_hal p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/hal/include
    ${d_SOC_PLATFORM_DIR}/hal/src
)
######################################## dep_hal end ##############################################


###################################### dep_misc begin #############################################
ameba_target_include(dep_${d_MCU_PROJECT_NAME}_misc p_SCOPE interface ${d_SOC_PLATFORM_DIR}/misc)
####################################### dep_misc end ##############################################

###################################### dep_rtk_coex begin #############################################
ameba_target_include(dep_${d_MCU_PROJECT_NAME}_rtk_coex p_SCOPE interface ${c_COMPONENT_DIR}/rtk_coex)
ameba_target_include(dep_${d_MCU_PROJECT_NAME}_rtk_coex_api p_SCOPE interface ${c_COMPONENT_DIR}/rtk_coex)

if(CONFIG_WLAN)
    if (CONFIG_AS_INIC_NP OR CONFIG_SINGLE_CORE_WIFI)
        if(CONFIG_MP_INCLUDED)
            if(CONFIG_MP_SHRINK)
                ameba_target_link(dep_${d_MCU_PROJECT_NAME}_rtk_coex p_SCOPE interface p_WHOLE_ARCHIVE
                    ${d_SDK_LIB_APPLICATION_DIR}/lib_coex_mp_shrink.a
                )
            else()
                ameba_target_link(dep_${d_MCU_PROJECT_NAME}_rtk_coex p_SCOPE interface p_WHOLE_ARCHIVE
                    ${d_SDK_LIB_APPLICATION_DIR}/lib_coex_mp.a
                )
            endif()
        else()
            ameba_target_link(dep_${d_MCU_PROJECT_NAME}_rtk_coex p_SCOPE interface p_WHOLE_ARCHIVE
                ${d_SDK_LIB_APPLICATION_DIR}/lib_coex.a
            )
        endif()
    endif()
endif()

# coex api
if(CONFIG_CORE_AS_AP OR ((NOT CONFIG_CORE_AS_NP) AND CONFIG_FULLMAC_MENU))
    ameba_target_link(dep_${d_MCU_PROJECT_NAME}_rtk_coex_api p_SCOPE interface p_WHOLE_ARCHIVE
        ${d_SDK_LIB_APPLICATION_DIR}/lib_coex_api.a
    )
endif()
####################################### dep_rtk_coex end ##############################################

###################################### dep_swlib begin ############################################
ameba_target_include(dep_${d_MCU_PROJECT_NAME}_swlib p_SCOPE interface ${d_SOC_PLATFORM_DIR}/swlib)
####################################### dep_swlib end #############################################


###################################### dep_test begin #############################################

####################################### dep_test end ##############################################

##################################### dep_usrcfg begin ############################################
ameba_target_include(dep_${d_MCU_PROJECT_NAME}_usrcfg p_SCOPE interface ${d_SOC_PLATFORM_DIR}/usrcfg/include)
###################################### dep_usrcfg end #############################################


###################################### dep_utils begin #############################################
ameba_target_link_if(CONFIG_IMQ_EN dep_${d_MCU_PROJECT_NAME}_utils p_SCOPE interface p_WHOLE_ARCHIVE
    ${d_SDK_LIB_APPLICATION_DIR}/lib_rpc.a
)
ameba_target_include(dep_${d_MCU_PROJECT_NAME}_utils p_SCOPE interface
    ${c_COMPONENT_DIR}/utils/ipc/ipc/${d_PLATFORM_TYPE}/include
    ${c_COMPONENT_DIR}/utils/ipc/rpc/hal/interfaces
    ${c_COMPONENT_DIR}/utils/ipc/rpc/include
    ${c_COMPONENT_DIR}/utils/ipc/rpc/xdr/include
)
####################################### dep_utils_ipc end ##############################################

###################################### dep_wifi begin #############################################

ameba_target_include(dep_${d_MCU_PROJECT_NAME}_wifi p_SCOPE interface
    ${c_CMPT_WIFI_DIR}/api
    ${c_CMPT_WIFI_DIR}/driver
    ${c_CMPT_WIFI_DIR}/driver/core
    ${c_CMPT_WIFI_DIR}/driver/core_rom
    ${c_CMPT_WIFI_DIR}/driver/halcommon_rom
    ${c_CMPT_WIFI_DIR}/driver/include
    ${c_CMPT_WIFI_DIR}/wifi_reg/common
    ${c_CMPT_WIFI_DIR}/driver/intf
    ${c_CMPT_WIFI_DIR}/driver/halmac
    ${c_CMPT_WIFI_DIR}/driver/halcommon
    ${c_CMPT_WIFI_DIR}/driver/halbtc
    ${c_CMPT_WIFI_DIR}/driver/phl
    ${c_CMPT_WIFI_DIR}/driver/p2p
    ${c_CMPT_WIFI_DIR}/wpa_supplicant/src
    ${c_CMPT_WIFI_DIR}/wpa_supplicant/src/crypto
    ${c_CMPT_WIFI_DIR}/wpa_supplicant/wpa_supplicant
    ${c_CMPT_WIFI_DIR}/wifi_fast_connect
    ${c_CMPT_WIFI_DIR}/wifi_auto_reconnect
    ${c_CMPT_WIFI_DIR}/simple_config
    ${c_CMPT_WIFI_DIR}/wifi_cast
    ${c_CMPT_WIFI_DIR}/common
    ${c_CMPT_WIFI_DIR}/common/secure
    ${c_CMPT_WIFI_DIR}/wpa_supplicant/wpa_lite
    ${c_CMPT_WIFI_DIR}/wpa_supplicant/wpa_lite/rom
    ${c_CMPT_WIFI_DIR}/driver/nan
    ${c_CMPT_WIFI_DIR}/driver/mesh
    ${c_CMPT_WIFI_DIR}/driver/wifi_tunnel
    ${c_CMPT_WIFI_DIR}/inic
    ${c_CMPT_WIFI_DIR}/inic/ipc
    ${c_CMPT_WIFI_DIR}/inic/common
    ${c_CMPT_WIFI_DIR}/inic/spi
    ${c_CMPT_WIFI_DIR}/inic/sdio
    ${c_CMPT_WIFI_DIR}/inic/usb
    ${c_CMPT_WIFI_DIR}/wifi_tunnel_app
)
if (${d_PLATFORM_TYPE} STREQUAL "amebax")
    ameba_target_include(dep_${d_MCU_PROJECT_NAME}_wifi p_SCOPE interface
        ${c_CMPT_WIFI_DIR}/wifi_reg/rle1331
        ${c_CMPT_WIFI_DIR}/driver/intf/g6_intf
        ${c_CMPT_WIFI_DIR}/driver/intf/g6_intf/rle1331
        ${c_CMPT_WIFI_DIR}/driver/phy_g6/bb
        ${c_CMPT_WIFI_DIR}/driver/phy_g6/bb/halbb_8720f
        ${c_CMPT_WIFI_DIR}/driver/phy_g6/rf
        ${c_CMPT_WIFI_DIR}/driver/rle1331
        ${c_CMPT_WIFI_DIR}/driver/rle1331_rom
    )
elseif(${d_PLATFORM_TYPE} STREQUAL "amebadplus")
    ameba_target_include(dep_${d_MCU_PROJECT_NAME}_wifi p_SCOPE interface
        ${c_CMPT_WIFI_DIR}/wifi_reg/rtl8721da
        ${c_CMPT_WIFI_DIR}/driver/rtl8721da
        ${c_CMPT_WIFI_DIR}/driver/rtl8721da_rom
         ${c_CMPT_WIFI_DIR}/driver/phydm
        ${c_CMPT_WIFI_DIR}/driver/wifi_tunnel/include
        ${c_CMPT_WIFI_DIR}/driver/wifi_tunnel/rtl8721da
    )
elseif(CONFIG_AMEBASMART)
    ameba_target_include(dep_${d_MCU_PROJECT_NAME}_wifi p_SCOPE interface
        ${c_CMPT_WIFI_DIR}/driver/phy_g6/bb
        ${c_CMPT_WIFI_DIR}/driver/phy_g6/bb/halbb_8730e
        ${c_CMPT_WIFI_DIR}/driver/phy_g6/rf
        ${c_CMPT_WIFI_DIR}/driver/rtl8730e
        ${c_CMPT_WIFI_DIR}/driver/rtl8730e_rom
        ${c_CMPT_WIFI_DIR}/wifi_reg/rtl8730e
        ${c_CMPT_WIFI_DIR}/driver/intf/g6_intf
        ${c_CMPT_WIFI_DIR}/driver/intf/g6_intf/rtl8730e
    )
elseif(${d_PLATFORM_TYPE} STREQUAL "amebalite")
    ameba_target_include(dep_${d_MCU_PROJECT_NAME}_wifi p_SCOPE interface
        ${c_CMPT_WIFI_DIR}/wifi_reg/rtl8720e
        ${c_CMPT_WIFI_DIR}/driver/intf/g6_intf
        ${c_CMPT_WIFI_DIR}/driver/intf/g6_intf/rtl8720e
        ${c_CMPT_WIFI_DIR}/driver/phy_g6/bb
        ${c_CMPT_WIFI_DIR}/driver/phy_g6/rf
        ${c_CMPT_WIFI_DIR}/driver/phy_g6/bb/halbb_8720e
        ${c_CMPT_WIFI_DIR}/driver/phy_g6/rf/halrf_8720e
        ${c_CMPT_WIFI_DIR}/driver/rtl8720e
        ${c_CMPT_WIFI_DIR}/driver/rtl8720e_rom
    )
elseif ("${d_PLATFORM_TYPE}" STREQUAL "amebagreen2" OR "${d_PLATFORM_TYPE}" STREQUAL "amebapro3")
    ameba_target_include(dep_${d_MCU_PROJECT_NAME}_wifi p_SCOPE interface
        ${c_CMPT_WIFI_DIR}/wifi_reg/rtl7005
        ${c_CMPT_WIFI_DIR}/driver/intf/g6_intf
        ${c_CMPT_WIFI_DIR}/driver/intf/g6_intf/rtl8720f
        ${c_CMPT_WIFI_DIR}/driver/phy_g6/bb
        ${c_CMPT_WIFI_DIR}/driver/phy_g6/bb/halbb_8720f
        ${c_CMPT_WIFI_DIR}/driver/phy_g6/rf
        ${c_CMPT_WIFI_DIR}/driver/rtl8720f
        ${c_CMPT_WIFI_DIR}/driver/rtl8720f_rom
    )
elseif(${d_PLATFORM_TYPE} STREQUAL "amebad")
    ameba_target_include(dep_${d_MCU_PROJECT_NAME}_wifi p_SCOPE interface
        ${c_CMPT_WIFI_DIR}/wifi_reg/rtl8721d
        ${c_CMPT_WIFI_DIR}/driver/rtl8721d
        ${c_CMPT_WIFI_DIR}/driver/rtl8721d_rom
        ${c_CMPT_WIFI_DIR}/driver/phydm
    )
endif()

ameba_target_include(dep_${d_MCU_PROJECT_NAME}_wifi p_SCOPE interface
    ${c_CMPT_WIFI_DIR}/wifi_fw/common
    ${c_CMPT_WIFI_DIR}/wifi_fw/${d_PLATFORM_TYPE}/include
)

if(CONFIG_WLAN)
    if(${d_PLATFORM_TYPE} STREQUAL "amebadplus")
        if (CONFIG_AS_INIC_NP)
            if(CONFIG_KM4_AS_NP)
                ameba_target_link(dep_${d_MCU_PROJECT_NAME}_wifi p_SCOPE interface p_WHOLE_ARCHIVE
                    ${d_SDK_LIB_APPLICATION_DIR}/lib_wifi_com_sec.a
                )
            elseif(CONFIG_ZEPHYR_SDK)
                ameba_target_link(dep_${d_MCU_PROJECT_NAME}_wifi p_SCOPE interface p_WHOLE_ARCHIVE
                    ${d_SDK_LIB_APPLICATION_DIR}/lib_wifi_common.a
                    ${d_SDK_LIB_APPLICATION_DIR}/lib_wpa_lite.a
                    ${d_SDK_LIB_APPLICATION_DIR}/lib_wps.a
                )
            else()
                ameba_target_link(dep_${d_MCU_PROJECT_NAME}_wifi p_SCOPE interface p_WHOLE_ARCHIVE
                    ${d_SDK_LIB_APPLICATION_DIR}/lib_wifi_common.a
                )
            endif()
        else()
            ameba_target_link(dep_${d_MCU_PROJECT_NAME}_wifi p_SCOPE interface p_WHOLE_ARCHIVE
                ${d_SDK_LIB_APPLICATION_DIR}/lib_wifi_com_sec.a
            )
        endif()
    elseif(CONFIG_AMEBASMART)
        ameba_target_link(dep_${d_MCU_PROJECT_NAME}_wifi p_SCOPE interface p_WHOLE_ARCHIVE
            ${d_SDK_LIB_APPLICATION_DIR}/lib_wifi_com_sec.a
        )
        if("${d_MCU_PROJECT_NAME}" STREQUAL "lp")
            ameba_target_link_if(CONFIG_WIFI_FW_EN dep_${d_MCU_PROJECT_NAME}_wifi p_SCOPE interface p_WHOLE_ARCHIVE
                "${d_SDK_LIB_APPLICATION_DIR}/lib_wifi_fw.a"
            )
        endif()
    elseif(${d_PLATFORM_TYPE} STREQUAL "amebalite")
        ameba_target_link(dep_${d_MCU_PROJECT_NAME}_wifi p_SCOPE interface p_WHOLE_ARCHIVE
            ${d_SDK_LIB_APPLICATION_DIR}/lib_wifi_com_sec.a
        )
    elseif("${d_PLATFORM_TYPE}" STREQUAL "amebagreen2" OR "${d_PLATFORM_TYPE}" STREQUAL "amebapro3")
        ameba_target_link(dep_${d_MCU_PROJECT_NAME}_wifi p_SCOPE interface p_WHOLE_ARCHIVE
            ${d_SDK_LIB_APPLICATION_DIR}/lib_wifi_common.a
        )
    elseif(${d_PLATFORM_TYPE} STREQUAL "amebad")
        ameba_target_link(dep_${d_MCU_PROJECT_NAME}_wifi p_SCOPE interface p_WHOLE_ARCHIVE
            ${d_SDK_LIB_APPLICATION_DIR}/lib_wifi_com_sec.a
        )
    else()

    endif()


    # AP Link Library
    if(CONFIG_AS_INIC_AP)
        if(NOT CONFIG_INIC_INTF_SPI)
            if(CONFIG_MP_INCLUDED)
                ameba_target_link_if(CONFIG_MP_SHRINK dep_${d_MCU_PROJECT_NAME}_wifi p_SCOPE interface p_WHOLE_ARCHIVE
                    ${d_SDK_LIB_APPLICATION_DIR}/lib_wifi_inic_ap_mp_shrink.a
                    p_ELSE
                    ${d_SDK_LIB_APPLICATION_DIR}/lib_wifi_inic_ap_mp.a
                )
            else()
                ameba_target_link(dep_${d_MCU_PROJECT_NAME}_wifi p_SCOPE interface  p_WHOLE_ARCHIVE
                    ${d_SDK_LIB_APPLICATION_DIR}/lib_wifi_inic_ap.a
                )
            endif()
        endif()
        ameba_target_link_ifnot(CONFIG_MP_SHRINK dep_${d_MCU_PROJECT_NAME}_wifi p_SCOPE interface p_WHOLE_ARCHIVE
            ${d_SDK_LIB_APPLICATION_DIR}/lib_wpa_lite.a
            ${d_SDK_LIB_APPLICATION_DIR}/lib_wps.a
        )
    # NP Link Library
    elseif(CONFIG_AS_INIC_NP)
        if(CONFIG_MP_INCLUDED)
            if(CONFIG_MP_SHRINK)
                if(CONFIG_INIC_INTF_SDIO)
                    ameba_target_link(dep_${d_MCU_PROJECT_NAME}_wifi p_SCOPE interface p_WHOLE_ARCHIVE
                        ${d_SDK_LIB_APPLICATION_DIR}/lib_wifi_inic_sdio_np_mp_shrink.a
                    )
                elseif(CONFIG_INIC_INTF_SPI)
                    ameba_target_link(dep_${d_MCU_PROJECT_NAME}_wifi p_SCOPE interface p_WHOLE_ARCHIVE
                        ${d_SDK_LIB_APPLICATION_DIR}/lib_wifi_inic_spi_np_mp_shrink.a
                    )
                elseif(CONFIG_INIC_INTF_USB)
                    ameba_target_link(dep_${d_MCU_PROJECT_NAME}_wifi p_SCOPE interface p_WHOLE_ARCHIVE
                        ${d_SDK_LIB_APPLICATION_DIR}/lib_wifi_inic_usb_np_mp_shrink.a
                    )
                else()
                    ameba_target_link(dep_${d_MCU_PROJECT_NAME}_wifi p_SCOPE interface p_WHOLE_ARCHIVE
                        ${d_SDK_LIB_APPLICATION_DIR}/lib_wifi_inic_np_mp_shrink.a
                    )
                endif()
            else()
                if(CONFIG_INIC_INTF_SDIO)
                    ameba_target_link(dep_${d_MCU_PROJECT_NAME}_wifi p_SCOPE interface p_WHOLE_ARCHIVE
                        ${d_SDK_LIB_APPLICATION_DIR}/lib_wifi_inic_sdio_np_mp.a
                    )
                elseif(CONFIG_INIC_INTF_SPI)
                    ameba_target_link(dep_${d_MCU_PROJECT_NAME}_wifi p_SCOPE interface p_WHOLE_ARCHIVE
                        ${d_SDK_LIB_APPLICATION_DIR}/lib_wifi_inic_spi_np_mp.a
                    )
                elseif(CONFIG_INIC_INTF_USB)
                    ameba_target_link(dep_${d_MCU_PROJECT_NAME}_wifi p_SCOPE interface p_WHOLE_ARCHIVE
                        ${d_SDK_LIB_APPLICATION_DIR}/lib_wifi_inic_usb_np_mp.a
                    )
                else()
                    ameba_target_link(dep_${d_MCU_PROJECT_NAME}_wifi p_SCOPE interface p_WHOLE_ARCHIVE
                        ${d_SDK_LIB_APPLICATION_DIR}/lib_wifi_inic_np_mp.a
                    )
                endif()
            endif()
        else()
            if(CONFIG_INIC_INTF_SDIO)
                ameba_target_link(dep_${d_MCU_PROJECT_NAME}_wifi p_SCOPE interface p_WHOLE_ARCHIVE
                    ${d_SDK_LIB_APPLICATION_DIR}/lib_wifi_inic_sdio_np.a
                )
            elseif(CONFIG_INIC_INTF_SPI)
                ameba_target_link(dep_${d_MCU_PROJECT_NAME}_wifi p_SCOPE interface p_WHOLE_ARCHIVE
                    ${d_SDK_LIB_APPLICATION_DIR}/lib_wifi_inic_spi_np.a
                )
            elseif(CONFIG_INIC_INTF_USB)
                ameba_target_link(dep_${d_MCU_PROJECT_NAME}_wifi p_SCOPE interface p_WHOLE_ARCHIVE
                    ${d_SDK_LIB_APPLICATION_DIR}/lib_wifi_inic_usb_np.a
                )
            else()
                ameba_target_link(dep_${d_MCU_PROJECT_NAME}_wifi p_SCOPE interface p_WHOLE_ARCHIVE
                    ${d_SDK_LIB_APPLICATION_DIR}/lib_wifi_inic_np.a
                )
            endif()
        endif()
    # Single Core Link Library
    elseif(CONFIG_SINGLE_CORE_WIFI)
        ameba_target_link_ifnot(CONFIG_MP_SHRINK dep_${d_MCU_PROJECT_NAME}_wifi p_SCOPE interface p_WHOLE_ARCHIVE
            ${d_SDK_LIB_APPLICATION_DIR}/lib_wpa_lite.a
            ${d_SDK_LIB_APPLICATION_DIR}/lib_wps.a
        )
        if(CONFIG_SDIO_BRIDGE)
            if(CONFIG_MP_INCLUDED)
                ameba_target_link_if(CONFIG_MP_SHRINK dep_${d_MCU_PROJECT_NAME}_wifi p_SCOPE interface p_WHOLE_ARCHIVE
                    ${d_SDK_LIB_APPLICATION_DIR}/lib_wifi_sdio_bridge_mp_shrink.a
                    p_ELSE
                    ${d_SDK_LIB_APPLICATION_DIR}/lib_wifi_sdio_bridge_mp.a
                )
            else()
                ameba_target_link(dep_${d_MCU_PROJECT_NAME}_wifi p_SCOPE interface p_WHOLE_ARCHIVE
                    ${d_SDK_LIB_APPLICATION_DIR}/lib_wifi_sdio_bridge.a
                )
            endif()
        else()
            if(CONFIG_MP_INCLUDED)
                ameba_target_link_if(CONFIG_MP_SHRINK dep_${d_MCU_PROJECT_NAME}_wifi p_SCOPE interface p_WHOLE_ARCHIVE
                    ${d_SDK_LIB_APPLICATION_DIR}/lib_wifi_single_core_mp_shrink.a
                    p_ELSE
                    ${d_SDK_LIB_APPLICATION_DIR}/lib_wifi_single_core_mp.a
                )
            else()
                ameba_target_link(dep_${d_MCU_PROJECT_NAME}_wifi p_SCOPE interface p_WHOLE_ARCHIVE
                    ${d_SDK_LIB_APPLICATION_DIR}/lib_wifi_single_core.a
                )
            endif()
        endif()
    else()
    endif()

    if(CONFIG_AS_INIC_AP)
        ameba_target_link_ifnot(CONFIG_MP_SHRINK dep_${d_MCU_PROJECT_NAME}_wifi p_SCOPE interface p_WHOLE_ARCHIVE
            ${d_SDK_LIB_APPLICATION_DIR}/lib_wifi_cast.a
        )
    endif()
else()
    if(CONFIG_AMEBASMART)
        #FIXME: for lp, lib_wifi_fw.a is required even though CONFIG_WLAN not defined
        if("${d_MCU_PROJECT_NAME}" STREQUAL "lp")
            ameba_target_link_if(CONFIG_WIFI_FW_EN dep_${d_MCU_PROJECT_NAME}_wifi p_SCOPE interface p_WHOLE_ARCHIVE
                ${d_SDK_LIB_APPLICATION_DIR}/lib_wifi_fw.a
            )
        endif()
    endif()
endif()
####################################### dep_wifi end #############################################

###################################### dep_img3 begin ############################################

####################################### dep_img3 end #############################################
