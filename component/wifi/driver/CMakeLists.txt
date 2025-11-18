##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin

# You may use if-else condition to set or update predefined variable above


ameba_list_append(public_definitions DM_ODM_SUPPORT_TYPE=32)

ameba_list_append(public_includes
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/halcommon
    ${CMAKE_CURRENT_SOURCE_DIR}/halbtc
    ${CMAKE_CURRENT_SOURCE_DIR}/phl
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/intf
    ${CMAKE_CURRENT_SOURCE_DIR}/core_rom
    ${CMAKE_CURRENT_SOURCE_DIR}/halcommon_rom
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/halmac
    ${CMAKE_CURRENT_SOURCE_DIR}/p2p
    ${CMAKE_CURRENT_SOURCE_DIR}/nan
    ${CMAKE_CURRENT_SOURCE_DIR}/mesh
    ${CMAKE_CURRENT_SOURCE_DIR}/wifi_tunnel
    ${c_CMPT_BLUETOOTH_DIR}/driver/bt_inic
    ${c_CMPT_WIFI_DIR}/wifi_reg/common
    ${c_CMPT_WIFI_DIR}/wifi_fw/common
    ${c_CMPT_WIFI_DIR}/wifi_fw/${c_SOC_TYPE}/include
)

ameba_list_append_if(CONFIG_WHC_INTF_SDIO public_includes
    ${c_CMPT_WIFI_DIR}/whc/whc_dev/sdio
    ${c_CMPT_WIFI_DIR}/whc/whc_host_rtos/sdio
)

ameba_list_append_if(CONFIG_WHC_INTF_SPI public_includes
    ${c_CMPT_WIFI_DIR}/whc/whc_dev/spi
    ${c_CMPT_WIFI_DIR}/whc/whc_host_rtos/spi
)

ameba_list_append_if(CONFIG_WHC_INTF_USB public_includes
    ${c_CMPT_WIFI_DIR}/whc/whc_dev/usb
)

ameba_list_append_if(CONFIG_WHC_INTF_IPC public_includes
    ${c_CMPT_WIFI_DIR}/whc/whc_dev/ipc
    ${c_CMPT_WIFI_DIR}/whc/whc_host_rtos/ipc
)

ameba_list_append_if(CONFIG_AMEBALITE public_includes
    ${c_CMPT_WIFI_DIR}/wifi_reg/rtl8720e
    ${CMAKE_CURRENT_SOURCE_DIR}/intf/g6_intf
    ${CMAKE_CURRENT_SOURCE_DIR}/intf/g6_intf/rtl8720e
    ${CMAKE_CURRENT_SOURCE_DIR}/phy_g6/bb
    ${CMAKE_CURRENT_SOURCE_DIR}/phy_g6/rf
    ${CMAKE_CURRENT_SOURCE_DIR}/phy_g6/bb/halbb_8720e
    ${CMAKE_CURRENT_SOURCE_DIR}/phy_g6/rf/halrf_8720e
    ${CMAKE_CURRENT_SOURCE_DIR}/rtl8720e
    ${CMAKE_CURRENT_SOURCE_DIR}/rtl8720e_rom
)

ameba_list_append_if(CONFIG_AMEBADPLUS public_includes
    ${c_CMPT_WIFI_DIR}/wifi_reg/rtl8721da
    ${CMAKE_CURRENT_SOURCE_DIR}/rtl8721da
    ${CMAKE_CURRENT_SOURCE_DIR}/rtl8721da_rom
    ${CMAKE_CURRENT_SOURCE_DIR}/phydm
    ${CMAKE_CURRENT_SOURCE_DIR}/wifi_tunnel/include
    ${CMAKE_CURRENT_SOURCE_DIR}/wifi_tunnel/rtl8721da
)

ameba_list_append_if(CONFIG_AMEBAD public_includes
    ${c_CMPT_WIFI_DIR}/wifi_reg/rtl8721d
    ${CMAKE_CURRENT_SOURCE_DIR}/rtl8721d
    ${CMAKE_CURRENT_SOURCE_DIR}/rtl8721d_rom
    ${CMAKE_CURRENT_SOURCE_DIR}/phydm
)

ameba_list_append_if(CONFIG_AMEBASMART public_includes
    ${c_CMPT_WIFI_DIR}/wifi_reg/rtl8730e
    ${CMAKE_CURRENT_SOURCE_DIR}/intf/g6_intf
    ${CMAKE_CURRENT_SOURCE_DIR}/intf/g6_intf/rtl8730e
    ${CMAKE_CURRENT_SOURCE_DIR}/rtl8730e_rom
    ${CMAKE_CURRENT_SOURCE_DIR}/phy_g6/bb
    ${CMAKE_CURRENT_SOURCE_DIR}/phy_g6/bb/halbb_8730e
    ${CMAKE_CURRENT_SOURCE_DIR}/phy_g6/rf
    ${CMAKE_CURRENT_SOURCE_DIR}/rtl8730e
    ${CMAKE_CURRENT_SOURCE_DIR}/phy_g6
)

ameba_list_append_if(CONFIG_AMEBAGREEN2 public_includes
    ${c_CMPT_WIFI_DIR}/wifi_reg/rtl7005
    ${CMAKE_CURRENT_SOURCE_DIR}/intf/g6_intf
    ${CMAKE_CURRENT_SOURCE_DIR}/intf/g6_intf/rtl8721f
    ${CMAKE_CURRENT_SOURCE_DIR}/phy_g6/bb
    ${CMAKE_CURRENT_SOURCE_DIR}/phy_g6/bb/halbb_8721f
    ${CMAKE_CURRENT_SOURCE_DIR}/phy_g6/rf
    ${CMAKE_CURRENT_SOURCE_DIR}/rtl8721f
    ${CMAKE_CURRENT_SOURCE_DIR}/rtl8721f_rom
    ${CMAKE_CURRENT_SOURCE_DIR}/wifi_tunnel/rtl8721f
)

ameba_list_append_if(CONFIG_AMEBAPRO3 public_includes
    ${c_CMPT_WIFI_DIR}/wifi_reg/rtl7005
    ${CMAKE_CURRENT_SOURCE_DIR}/intf/g6_intf
    ${CMAKE_CURRENT_SOURCE_DIR}/intf/g6_intf/rtl8721f
    ${CMAKE_CURRENT_SOURCE_DIR}/phy_g6/bb
    ${CMAKE_CURRENT_SOURCE_DIR}/phy_g6/bb/halbb_8721f
    ${CMAKE_CURRENT_SOURCE_DIR}/phy_g6/rf
    ${CMAKE_CURRENT_SOURCE_DIR}/rtl8721f
    ${CMAKE_CURRENT_SOURCE_DIR}/rtl8721f_rom
)

ameba_list_append_if(CONFIG_AMEBAL2 public_includes
    ${c_CMPT_WIFI_DIR}/wifi_reg/rle1331
    ${CMAKE_CURRENT_SOURCE_DIR}/intf/g6_intf
    ${CMAKE_CURRENT_SOURCE_DIR}/intf/g6_intf/rle1331
    ${CMAKE_CURRENT_SOURCE_DIR}/phy_g6/bb
    ${CMAKE_CURRENT_SOURCE_DIR}/phy_g6/bb/halbb_6955
    ${CMAKE_CURRENT_SOURCE_DIR}/phy_g6/rf
    ${CMAKE_CURRENT_SOURCE_DIR}/phy_g6/rf/halrf_6955
    ${CMAKE_CURRENT_SOURCE_DIR}/rle1331
    ${CMAKE_CURRENT_SOURCE_DIR}/rle1331_rom
)

ameba_list_append_if(CONFIG_RTL8720F public_includes
    ${c_CMPT_WIFI_DIR}/wifi_reg/rtl8720f
    ${CMAKE_CURRENT_SOURCE_DIR}/intf/g6_intf
    ${CMAKE_CURRENT_SOURCE_DIR}/intf/g6_intf/rtl8720f
    ${CMAKE_CURRENT_SOURCE_DIR}/phy_g6/bb
    ${CMAKE_CURRENT_SOURCE_DIR}/phy_g6/bb/halbb_6955
    ${CMAKE_CURRENT_SOURCE_DIR}/phy_g6/rf
    ${CMAKE_CURRENT_SOURCE_DIR}/phy_g6/rf/halrf_6955
    ${CMAKE_CURRENT_SOURCE_DIR}/rtl8720f
    ${CMAKE_CURRENT_SOURCE_DIR}/rtl8720f_rom
)

set(release_target_name wlan)

if(CONFIG_MP_INCLUDED)
    if(CONFIG_MP_SHRINK)
        set(LIB_NAME_MP_SUFFIX _mp_shrink)
    else()
        set(LIB_NAME_MP_SUFFIX _mp)
    endif()
endif()

if(CONFIG_WHC_WPA_SUPPLICANT_OFFLOAD)
    set(LIB_NAME_WPA_OFFLOAD_SUFFIX _wpaoffload)
endif()

if(CONFIG_WHC_DEV)
    if(CONFIG_WHC_INTF_SDIO)
        set(release_target_name wifi_whc_sdio_np${LIB_NAME_WPA_OFFLOAD_SUFFIX}${LIB_NAME_MP_SUFFIX})
    elseif(CONFIG_WHC_INTF_SPI)
        set(release_target_name wifi_whc_spi_np${LIB_NAME_WPA_OFFLOAD_SUFFIX}${LIB_NAME_MP_SUFFIX})
    elseif(CONFIG_WHC_INTF_USB)
        set(release_target_name wifi_whc_usb_np${LIB_NAME_WPA_OFFLOAD_SUFFIX}${LIB_NAME_MP_SUFFIX})
    else()
        set(release_target_name wifi_whc_np${LIB_NAME_WPA_OFFLOAD_SUFFIX}${LIB_NAME_MP_SUFFIX})
    endif()
elseif(CONFIG_WHC_NONE)
    set(release_target_name wifi_single_core${LIB_NAME_WPA_OFFLOAD_SUFFIX}${LIB_NAME_MP_SUFFIX})
endif()

if(CONFIG_WLAN AND (CONFIG_WHC_DEV OR CONFIG_WHC_NONE))
    ameba_list_append_if(CONFIG_WLAN public_libraries
        ${c_SDK_LIB_APPLICATION_DIR}/lib_${release_target_name}.a
    )
endif()

# Component public part, user config end
#----------------------------------------#

#WARNING: Fixed section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_definitions})
ameba_global_library(${public_libraries}) #default: whole-archived

##########################################################################################
## * This part defines private part of the component
## * Private part is used to build target of current component
## * NOTE: The build API guarantees the global build configures(mentioned above)
## *       applied to the target automatically. So if any configure was already added
## *       to public above, it's unnecessary to add again below.

#NOTE: User defined section, add your private build configures here
# You may use if-else condition to set these predefined variable
# They are only for ameba_add_internal_library/ameba_add_external_app_library/ameba_add_external_soc_library
set(private_sources)                 #private source files, NOTE: relative path is OK
set(private_includes)                #private include directories, NOTE: relative path is OK
set(private_definitions)             #private definitions
set(private_compile_options)         #private compile_options

#------------------------------#
# Component private part, user config begin

set(CSRC_DRV)
set(CSRC_PHY)
set(CSRC_BTC)

include(drv.cmake OPTIONAL)
include(drv_common.cmake OPTIONAL)
include(phydm/phydm.cmake OPTIONAL)
include(phy_g6/phy_g6.cmake OPTIONAL)

ameba_list_append(private_sources ${CSRC_DRV} ${CSRC_PHY} ${CSRC_BTC})

ameba_list_append_if(CONFIG_AMEBALITE private_includes
    ${c_CMPT_WIFI_DIR}/wifi_fw/amebalite/include
)

ameba_list_append_if(CONFIG_AMEBADPLUS private_includes
    ${c_CMPT_WIFI_DIR}/wifi_fw/amebadplus/include
    ${c_FREERTOS_DIR}/include
    ${c_FREERTOS_DIR}/portable/GCC/AmebaDplus_KM0
)

ameba_list_append_if(CONFIG_AMEBAD private_includes
    ${c_CMPT_WIFI_DIR}/wifi_fw/amebad/include
    ${c_FREERTOS_DIR}/include
    ${c_FREERTOS_DIR}/portable/GCC/AmebaD_KM4/non_secure
    ${c_FREERTOS_DIR}/portable/GCC/AmebaD_KM4/secure
)

ameba_list_append_if(CONFIG_AMEBASMART private_includes
    ${c_CMPT_WIFI_DIR}/wifi_fw/amebasmart/include
    ${c_FREERTOS_DIR}/include/
    ${c_FREERTOS_DIR}/portable/GCC/AmebaSmart_HP/non_secure
    ${c_FREERTOS_DIR}/portable/GCC/AmebaSmart_HP/secure
    ${c_CMPT_FWLIB_DIR}/ap_core
)

ameba_list_append_if(CONFIG_AMEBAGREEN2 private_includes
    ${c_COMPONENT_DIR}/soc/amebagreen2/rom/include
    ${c_CMPT_WIFI_DIR}/wifi_fw/amebagreen2/include
)

ameba_list_append_if(CONFIG_AMEBAPRO3 private_includes
    ${c_COMPONENT_DIR}/soc/amebapro3/rom/include
    ${c_CMPT_WIFI_DIR}/wifi_fw/amebagreen2/include
)

ameba_list_append_if(CONFIG_AMEBAL2 private_includes
    ${c_COMPONENT_DIR}/soc/amebaL2/rom/include
    ${c_CMPT_WIFI_DIR}/wifi_fw/rle1331/include
)

ameba_list_append_if(CONFIG_RTL8720F private_includes
    ${c_COMPONENT_DIR}/soc/RTL8720F/rom/include
    ${c_CMPT_WIFI_DIR}/wifi_fw/rle1331/include
)

ameba_list_append(private_definitions COMPIL_LOG_LEVEL=RTK_LOG_INFO)

ameba_list_append_if(CONFIG_MP_INCLUDED private_compile_options -Wno-unused-function)

# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone
if(CONFIG_WLAN AND (CONFIG_WHC_DEV OR CONFIG_WHC_NONE))
    ###NOTE: For closed-source component, only build before release and libs are packaged into lib/application
    ameba_add_external_app_library(${release_target_name}
        p_SOURCES
            ${private_sources}
        p_INCLUDES
            ${private_includes}
        p_DEFINITIONS
            ${private_definitions}
        p_COMPILE_OPTIONS
            ${private_compile_options}
    )
endif()
##########################################################################################
