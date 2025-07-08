##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin

# You may use if-else condition to set or update predefined variable above
set(release_target_name)

if(CONFIG_WHC_HOST AND CONFIG_WHC_INTF_IPC)
    if(CONFIG_MP_INCLUDED)
        if(CONFIG_MP_SHRINK)
            set(release_target_name wifi_whc_ap_mp_shrink)
        else()
            set(release_target_name wifi_whc_ap_mp)
        endif()
    else()
        set(release_target_name wifi_whc_ap)
    endif()
endif()

if(CONFIG_WLAN AND CONFIG_WHC_HOST AND CONFIG_WHC_INTF_IPC)
    ameba_list_append(public_libraries
        ${c_SDK_LIB_APPLICATION_DIR}/lib_${release_target_name}.a
    )
endif()

ameba_list_append(public_includes
    ${CMAKE_CURRENT_SOURCE_DIR}
    whc_dev
    whc_host_rtos
    whc_dev/spi
    whc_dev/sdio
    whc_dev/usb
    whc_dev/ipc
    whc_host_rtos/spi
    whc_host_rtos/ipc
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
set(is_internal_library TRUE)
set(target_name)

if(CONFIG_WIFI_HOST_CONTROL)
    ameba_list_append(private_sources
        whc_dev/whc_dev_msg_queue.c
        whc_dev/whc_dev_trx.c
        whc_dev/whc_dev_protocal_offload.c
    )
    if(CONFIG_WHC_DUAL_TCPIP)
        ameba_list_append(private_sources
            whc_dev/whc_dev_bridge.c
        )
    endif()

    if(CONFIG_WHC_WIFI_API_PATH)
        ameba_list_append(private_sources
            whc_dev/whc_dev_api_path.c
            whc_dev/whc_dev_cust_evt.c
        )
    endif()

    if(CONFIG_WHC_CMD_PATH)
        ameba_list_append(private_sources
            whc_dev/whc_dev_app.c
            whc_dev/whc_dev_api.c
        )
    endif()
endif()

if(CONFIG_WHC_DEV)
    if(CONFIG_WHC_INTF_SPI)
        ameba_list_append(private_sources
            whc_dev/spi/whc_spi_drv.c
            whc_dev/spi/whc_spi_dev.c
        )
        set(target_name wifi_whc_spi_dev)
    elseif(CONFIG_WHC_INTF_SDIO)
        ameba_list_append(private_sources
            whc_dev/sdio/whc_sdio_drv.c
            whc_dev/sdio/whc_sdio_dev.c
        )
        set(target_name wifi_whc_sdio_dev)
    elseif(CONFIG_WHC_INTF_USB)
        ameba_list_append(private_sources
            whc_dev/usb/whc_usb_dev.c
        )
        set(target_name wifi_whc_usb_dev)
    endif()
elseif(CONFIG_WHC_BRIDGE)
    if(CONFIG_WHC_INTF_SDIO)
        ameba_list_append(private_sources
            whc_dev/sdio/whc_sdio_drv.c
            whc_dev/sdio/whc_sdio_dev.c
        )
        if(CONFIG_WPA_LITE_OPS)
             ameba_list_append(private_sources
                 whc_dev/bridge_wpa_ops_demo/wpa_lite_intf/whc_wpa_lite_app.c
        )
        endif()
        set(target_name wifi_fullmac_sdio_bridge_lite)
    elseif(CONFIG_WHC_INTF_SPI)
         ameba_list_append(private_sources
            whc_dev/spi/whc_spi_drv.c
            whc_dev/spi/whc_spi_dev.c
        )
        set(target_name wifi_fullmac_spi_bridge_lite)
    endif()
elseif(CONFIG_WHC_HOST)
    if(CONFIG_WHC_INTF_SPI)
        ameba_list_append(private_sources
            whc_host_rtos/whc_host_init.c
            whc_host_rtos/spi/whc_spi_host.c
            whc_host_rtos/spi/whc_spi_host_trx.c
        )
        if(CONFIG_WHC_BRIDGE_HOST)
            ameba_list_append(private_sources
            whc_host_rtos/whc_bridge_host_app.c
            )
        else()
            ameba_list_append(private_sources
                whc_host_rtos/whc_host_api.c
                whc_host_rtos/whc_host_api_basic.c
                whc_host_rtos/whc_host_api_ext.c
                whc_host_rtos/whc_host_cust_evt.c
            )
        endif()
        set(target_name wifi_whc_spi_host)
    elseif(CONFIG_WHC_INTF_SDIO)
        ameba_list_append(private_sources
            whc_host_rtos/whc_host_init.c
            whc_host_rtos/sdio/rtw_sdio_drvio.c
            whc_host_rtos/sdio/whc_sdio_host.c
            whc_host_rtos/sdio/whc_sdio_host_trx.c
            whc_host_rtos/sdio/whc_sdio_init.c
        )
        if(CONFIG_WHC_BRIDGE_HOST)
            ameba_list_append(private_sources
                whc_host_rtos/whc_bridge_host_app.c
            )
        endif()
        set(target_name wifi_whc_sdio_host)
    else()
        ameba_list_append(private_sources
            whc_host_rtos/ipc/whc_ipc_host.c
            whc_host_rtos/ipc/whc_ipc_host_trx.c
            whc_host_rtos/ipc/whc_ipc_host_api.c
            whc_host_rtos/ipc/whc_ipc_host_api_basic.c
            whc_host_rtos/ipc/whc_ipc_host_api_ext.c
            whc_ipc_msg_queue.c
        )
        if(CONFIG_ZEPHYR_SDK)
            ameba_list_append(private_sources
                whc_host_rtos/ipc/whc_ipc_host_zephyr.c
            )
        endif()
        set(is_internal_library FALSE)
    endif()
endif()


ameba_list_append(private_includes
    ${c_CMPT_WIFI_DIR}/common
    ${c_CMPT_WIFI_DIR}/driver
    ${c_CMPT_WIFI_DIR}/driver/halmac
    ${c_CMPT_WIFI_DIR}/driver/halcommon
    ${c_CMPT_WIFI_DIR}/driver/halbtc
    ${c_CMPT_WIFI_DIR}/driver/phl
    ${c_CMPT_BLUETOOTH_DIR}/driver/bt_inic
)

if(CONFIG_AMEBALITE)
    ameba_list_append(private_includes
        ${c_CMPT_WIFI_DIR}/driver/phy_g6/bb
        ${c_CMPT_WIFI_DIR}/driver/phy_g6/rf
        ${c_CMPT_WIFI_DIR}/driver/rtl8720e
        ${c_CMPT_WIFI_DIR}/driver/rtl8720e_rom
        ${c_CMPT_WIFI_DIR}/wifi_fw/amebalite/include
    )
elseif(CONFIG_AMEBADPLUS)
    ameba_list_append(private_includes
        ${c_CMPT_WIFI_DIR}/driver/phydm
        ${c_CMPT_WIFI_DIR}/driver/rtl8721da
        ${c_CMPT_WIFI_DIR}/driver/rtl8721da_rom
        ${c_CMPT_WIFI_DIR}/wifi_fw/amebadplus/include
    )
elseif(CONFIG_AMEBASMART)
    ameba_list_append(private_includes
        ${c_CMPT_WIFI_DIR}/driver/phy_g6
        ${c_CMPT_WIFI_DIR}/driver/phy_g6/bb
        ${c_CMPT_WIFI_DIR}/driver/phy_g6/rf
        ${c_CMPT_WIFI_DIR}/driver/rtl8730e
        ${c_CMPT_WIFI_DIR}/driver/rtl8730e_rom
        ${c_CMPT_WIFI_DIR}/wifi_fw/amebasmart/include
    )

elseif(CONFIG_AMEBASMARTPLUS)
    ameba_list_append(private_includes
        ${c_CMPT_WIFI_DIR}/driver/phy_g6
        ${c_CMPT_WIFI_DIR}/driver/phy_g6/bb
        ${c_CMPT_WIFI_DIR}/driver/phy_g6/rf
        ${c_CMPT_WIFI_DIR}/driver/rtl8730e
        ${c_CMPT_WIFI_DIR}/driver/rtl8730e_rom
        ${c_CMPT_WIFI_DIR}/wifi_fw/amebasmart/include
    )
endif()

if(CONFIG_WHC_INTF_USB)
    ameba_list_append(private_includes
        ${c_CMPT_USB_DIR}/common
        ${c_CMPT_USB_DIR}/device/core
    )
    if(CONFIG_AMEBADPLUS)
        ameba_list_append(private_includes ${c_CMPT_USB_DIR}/device/inic_dplus)
    elseif(CONFIG_AMEBAGREEN2)
        ameba_list_append(private_includes ${c_CMPT_USB_DIR}/device/inic)
    endif()
endif()


if(CONFIG_WHC_BRIDGE)
    if(CONFIG_WHC_INTF_SDIO)
        if(CONFIG_WPA_LITE_OPS)
            ameba_list_append(private_includes ${c_CMPT_WIFI_DIR}/whc/whc_dev/bridge_wpa_ops_demo)
        endif()
    endif()
endif()

# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone
if(private_sources)
    if(is_internal_library)
        ###NOTE: For open-source component, always build from source
        ameba_add_internal_library(${target_name}
            p_SOURCES
                ${private_sources}
            p_INCLUDES
                ${private_includes}
            p_DEFINITIONS
                ${private_definitions}
            p_COMPILE_OPTIONS
                ${private_compile_options}
        )
    else()
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
endif()
##########################################################################################
