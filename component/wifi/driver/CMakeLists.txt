##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin

# You may use if-else condition to set or update predefined variable above


ameba_list_append(public_includes
    ${c_CMPT_WIFI_DIR}/common
    ${c_CMPT_WIFI_DIR}/driver
    ${c_CMPT_WIFI_DIR}/driver/halcommon
    ${c_CMPT_WIFI_DIR}/driver/halbtc
    ${c_CMPT_WIFI_DIR}/driver/phl
    ${c_CMPT_WIFI_DIR}/driver/core
    ${c_CMPT_WIFI_DIR}/driver/intf
    ${c_CMPT_WIFI_DIR}/common/secure
    ${c_CMPT_BLUETOOTH_DIR}/driver/bt_inic
    ${c_CMPT_WIFI_DIR}/whc
    ${c_CMPT_WIFI_DIR}/whc/whc_dev
    ${c_CMPT_WIFI_DIR}/whc/whc_host_rtos
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
    ${c_CMPT_WIFI_DIR}/phy_g6/bb
    ${c_CMPT_WIFI_DIR}/driver/phy_g6/bb/halbb_8720e
    ${c_CMPT_WIFI_DIR}/driver/phy_g6/rf
    ${c_CMPT_WIFI_DIR}/driver/rtl8720e
    ${c_CMPT_WIFI_DIR}/driver/rtl8720e_rom
    ${c_CMPT_WIFI_DIR}/wifi_fw/amebalite/include
)

ameba_list_append_if(CONFIG_AMEBADPLUS public_includes
    ${c_CMPT_WIFI_DIR}/driver/phydm
    ${c_CMPT_WIFI_DIR}/driver/rtl8721da
    ${c_CMPT_WIFI_DIR}/driver/rtl8721da_rom
    ${c_CMPT_WIFI_DIR}/wifi_fw/amebadplus/include
    ${c_FREERTOS_DIR}/include
    ${c_FREERTOS_DIR}/portable/GCC/AmebaDplus_KM0
)

ameba_list_append_if(CONFIG_AMEBAD public_includes
    ${c_CMPT_WIFI_DIR}/driver/phydm
    ${c_CMPT_WIFI_DIR}/driver/rtl8721d
    ${c_CMPT_WIFI_DIR}/driver/rtl8721d_rom
    ${c_CMPT_WIFI_DIR}/wifi_fw/amebad/include
    ${c_FREERTOS_DIR}/include
    ${c_FREERTOS_DIR}/portable/GCC/AmebaD_KM4/non_secure
    ${c_FREERTOS_DIR}/portable/GCC/AmebaD_KM4/secure
)

ameba_list_append_if(CONFIG_AMEBASMART public_includes
    ${c_CMPT_FWLIB_DIR}/ap_core
    ${c_CMPT_WIFI_DIR}/driver/phy_g6
    ${c_CMPT_WIFI_DIR}/driver/phy_g6/bb
    ${c_CMPT_WIFI_DIR}/driver/phy_g6/bb/halbb_8730e
    ${c_CMPT_WIFI_DIR}/driver/phy_g6/rf
    ${c_CMPT_WIFI_DIR}/driver/rtl8730e
    ${c_CMPT_WIFI_DIR}/driver/rtl8730e_rom
    ${c_CMPT_WIFI_DIR}/wifi_fw/amebasmart/include
    ${c_FREERTOS_DIR}/include/
    ${c_FREERTOS_DIR}/portable/GCC/AmebaSmart_HP/non_secure
    ${c_FREERTOS_DIR}/portable/GCC/AmebaSmart_HP/secure
)

ameba_list_append_if(CONFIG_AMEBASMARTPLUS public_includes
    ${c_CMPT_FWLIB_DIR}/ap_core
    ${c_CMPT_WIFI_DIR}/driver/phy_g6
    ${c_CMPT_WIFI_DIR}/driver/phy_g6/bb
    ${c_CMPT_WIFI_DIR}/driver/phy_g6/bb/halbb_8730e
    ${c_CMPT_WIFI_DIR}/driver/phy_g6/rf
    ${c_CMPT_WIFI_DIR}/driver/rtl8730e
    ${c_CMPT_WIFI_DIR}/driver/rtl8730e_rom
    ${c_CMPT_WIFI_DIR}/wifi_fw/amebasmart/include
    ${c_FREERTOS_DIR}/include/
    ${c_FREERTOS_DIR}/portable/GCC/AmebaSmart_HP/non_secure
    ${c_FREERTOS_DIR}/portable/GCC/AmebaSmart_HP/secure
)

ameba_list_append_if(CONFIG_AMEBAGREEN2 public_includes
    ${c_CMPT_WIFI_DIR}/driver/phy_g6/bb
    ${c_CMPT_WIFI_DIR}/driver/phy_g6/rf
    ${c_CMPT_WIFI_DIR}/driver/rtl8720f
    ${c_CMPT_WIFI_DIR}/driver/rtl8720f_rom
    ${c_COMPONENT_DIR}/soc/amebagreen2/rom/include
    ${c_CMPT_WIFI_DIR}/wifi_fw/amebagreen2/include
)

ameba_list_append_if(CONFIG_AMEBAPRO3 public_includes
    ${c_CMPT_WIFI_DIR}/driver/phy_g6/bb
    ${c_CMPT_WIFI_DIR}/driver/phy_g6/rf
    ${c_CMPT_WIFI_DIR}/driver/rtl8720f
    ${c_CMPT_WIFI_DIR}/driver/rtl8720f_rom
    ${c_COMPONENT_DIR}/soc/amebagreen2/rom/include
    ${c_CMPT_WIFI_DIR}/wifi_fw/amebagreen2/include
)

ameba_list_append_if(CONFIG_AMEBAL2 public_includes
    ${c_CMPT_WIFI_DIR}/driver/phy_g6/bb
    ${c_CMPT_WIFI_DIR}/driver/phy_g6/rf
    ${c_CMPT_WIFI_DIR}/driver/rle1331
    ${c_CMPT_WIFI_DIR}/driver/rle1331_rom
    ${c_COMPONENT_DIR}/soc/amebaL2/rom/include
    ${c_CMPT_WIFI_DIR}/wifi_fw/rle1331/include
)


set(release_target_name wlan)

if(CONFIG_MP_INCLUDED)
    if(CONFIG_MP_SHRINK)
        set(LIB_NAME_MP_SUFFIX _mp_shrink)
    else()
        set(LIB_NAME_MP_SUFFIX _mp)
    endif()
endif()

if(CONFIG_AS_INIC_NP)
    if(CONFIG_WHC_INTF_SDIO)
        set(release_target_name wifi_whc_sdio_np${LIB_NAME_MP_SUFFIX})
    elseif(CONFIG_WHC_INTF_SPI)
        set(release_target_name wifi_whc_spi_np${LIB_NAME_MP_SUFFIX})
    elseif(CONFIG_WHC_INTF_USB)
        set(release_target_name wifi_whc_usb_np${LIB_NAME_MP_SUFFIX})
    else()
        set(release_target_name wifi_whc_np${LIB_NAME_MP_SUFFIX})
    endif()
elseif(CONFIG_SINGLE_CORE_WIFI)
    if(CONFIG_WHC_BRIDGE)
        if(CONFIG_WHC_INTF_SDIO)
            set(release_target_name wifi_sdio_bridge${LIB_NAME_MP_SUFFIX})
        elseif(CONFIG_WHC_INTF_SPI)
            set(release_target_name wifi_spi_bridge${LIB_NAME_MP_SUFFIX})
        endif()
    else()
        set(release_target_name wifi_single_core${LIB_NAME_MP_SUFFIX})
    endif()
endif()

ameba_list_append_if(CONFIG_WLAN public_libraries
    ${c_SDK_LIB_APPLICATION_DIR}/lib_${release_target_name}.a
)

# Component public part, user config end
#----------------------------------------#

#WARNING: Fixed section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_defines})
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


set(CSRC_PHY)

include(drv.cmake OPTIONAL)
include(drv_common.cmake OPTIONAL)
include(phydm/phydm.cmake OPTIONAL)
include(phy_g6/phy_g6.cmake OPTIONAL)
include(halbtc/btc.cmake OPTIONAL)

ameba_list_append(private_sources ${CSRC_DRV} ${CSRC_PHY} ${CSRC_BTC})

ameba_list_append(private_definitions COMPIL_LOG_LEVEL=RTK_LOG_INFO)

ameba_list_append_if(CONFIG_MP_INCLUDED private_compile_options -Wno-unused-function)

# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

###NOTE: For closed-source component, only build before release and libs are packaged into lib/application
ameba_add_external_app_library(${release_target_name}
    p_ADD_BUILD_INFO
    p_SOURCES
        ${private_sources}
    p_INCLUDES
        ${private_includes}
    p_DEFINITIONS
        ${private_definitions}
    p_COMPILE_OPTIONS
        ${private_compile_options}
)
##########################################################################################