set(
    MODULE_IFLAGS 
    ${BASEDIR}/component/wifi/common
    ${BASEDIR}/component/wifi/driver
    ${BASEDIR}/component/wifi/driver/halmac
    ${BASEDIR}/component/wifi/driver/halcommon
    ${BASEDIR}/component/wifi/driver/halbtc
    ${BASEDIR}/component/wifi/driver/phl
    ${BASEDIR}/component/bluetooth/driver/bt_inic
)

if(CONFIG_AMEBALITE)
    list(
        APPEND MODULE_IFLAGS
        ${BASEDIR}/component/wifi/driver/phy_g6/bb
        ${BASEDIR}/component/wifi/driver/phy_g6/rf
        ${BASEDIR}/component/wifi/driver/rtl8720e
        ${BASEDIR}/component/wifi/driver/rtl8720e_rom
        ${BASEDIR}/component/wifi/wifi_fw/amebalite/include
    )
endif()

if(CONFIG_AMEBADPLUS)
    list(
        APPEND MODULE_IFLAGS
        ${BASEDIR}/component/wifi/driver/phydm
        ${BASEDIR}/component/wifi/driver/rtl8721da
        ${BASEDIR}/component/wifi/driver/rtl8721da_rom
        ${BASEDIR}/component/wifi/wifi_fw/amebadplus/include
    )
endif()

if(CONFIG_AMEBASMART)
    list(
        APPEND MODULE_IFLAGS
        ${BASEDIR}/component/wifi/driver/phy_g6
        ${BASEDIR}/component/wifi/driver/phy_g6/bb
        ${BASEDIR}/component/wifi/driver/phy_g6/rf
        ${BASEDIR}/component/wifi/driver/rtl8730e
        ${BASEDIR}/component/wifi/driver/rtl8730e_rom
        ${BASEDIR}/component/wifi/wifi_fw/amebasmart/include
    )
endif()

if(CONFIG_AMEBASMARTPLUS)
    list(
        APPEND MODULE_IFLAGS
        ${BASEDIR}/component/wifi/driver/phy_g6
        ${BASEDIR}/component/wifi/driver/phy_g6/bb
        ${BASEDIR}/component/wifi/driver/phy_g6/rf
        ${BASEDIR}/component/wifi/driver/rtl8730e
        ${BASEDIR}/component/wifi/driver/rtl8730e_rom
        ${BASEDIR}/component/wifi/wifi_fw/amebasmart/include
    )
endif()

set(DEVDIR ${BASEDIR}/component/wifi/whc/whc_dev)
set(HOSTDIR ${BASEDIR}/component/wifi/whc/whc_host_rtos)
set(COMDIR ${BASEDIR}/component/wifi/whc)


if(CONFIG_WHC_INTF_USB)
    list(
        APPEND MODULE_IFLAGS 
        ${BASEDIR}/component/usb/common
        ${BASEDIR}/component/usb/device/core
    )
    if(CONFIG_AMEBADPLUS)
        list( APPEND MODULE_IFLAGS ${BASEDIR}/component/usb/device/inic_dplus)
    elseif(CONFIG_AMEBAGREEN2)
        list(APPEND MODULE_IFLAGS ${BASEDIR}/component/usb/device/inic)
    endif()
endif()

set(CSRC)

if(CONFIG_WIFI_HOST_CONTROL)
        list(
            APPEND CSRC
            ${DEVDIR}/whc_dev_msg_queue.c
            ${DEVDIR}/whc_dev_trx.c
            ${DEVDIR}/whc_dev_protocal_offload.c
        )
       if(NOT CONFIG_FULLMAC_BRIDGE)
           list(
               APPEND CSRC
               ${DEVDIR}/whc_dev_api.c
               ${DEVDIR}/whc_dev_cust_evt.c
           )
       endif()
       if(NOT CONFIG_FULLMAC_MENU)
           list(
               APPEND CSRC
               ${DEVDIR}/whc_dev_bridge.c
               ${DEVDIR}/whc_bridge_dev_api.c
           )
       endif()
endif()

if(CONFIG_AS_INIC_NP)
    if(CONFIG_WHC_INTF_SPI)
        list(
            APPEND CSRC
            ${DEVDIR}/spi/whc_spi_dev.c
            ${DEVDIR}/spi/whc_fullmac_spi_dev.c
        )
        ameba_internal_library(wifi_whc_spi_dev)
        target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
        target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
    elseif(CONFIG_WHC_INTF_SDIO)
        list(
            APPEND CSRC
            ${DEVDIR}/sdio/whc_sdio_drv.c
            ${DEVDIR}/sdio/whc_fullmac_sdio_dev.c
        )
        ameba_internal_library(wifi_whc_sdio_dev)
        target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
        target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
    elseif(CONFIG_WHC_INTF_USB)
        list(
            APPEND CSRC
            ${DEVDIR}/usb/whc_usb_dev.c
        )
        ameba_internal_library(wifi_whc_usb_dev)
        target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
        target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
    endif()
elseif(CONFIG_FULLMAC_BRIDGEB)
    if(CONFIG_WHC_INTF_SDIO)
        list(
            APPEND CSRC
            ${DEVDIR}/sdio/whc_sdio_drv.c
            ${DEVDIR}/sdio/whc_fullmac_sdio_dev.c
        )
        ameba_internal_library(wifi_fullmac_sdio_bridge)
        target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
        target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
    elseif(CONFIG_WHC_INTF_SPI)
        list(
            APPEND CSRC
            ${DEVDIR}/spi/whc_spi_dev.c
        )
        ameba_internal_library(wifi_fullmac_spi_bridge)
        target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
        target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
    endif()
elseif(CONFIG_FULLMAC_BRIDGE)
    if(CONFIG_WHC_INTF_SDIO)
        list(
            APPEND CSRC
            ${DEVDIR}/sdio/whc_sdio_drv.c
            ${DEVDIR}/sdio/whc_bridge_sdio_dev.c
            ${DEVDIR}/whc_dev_app.c
        )
        ameba_internal_library(wifi_fullmac_sdio_bridge_lite)
        target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
        target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
    elseif(CONFIG_WHC_INTF_SPI)
         list(
            APPEND CSRC
            ${DEVDIR}/spi/whc_spi_dev.c
            ${DEVDIR}/spi/whc_bridge_spi_dev.c
            ${DEVDIR}/whc_dev_app.c
        )
        ameba_internal_library(wifi_fullmac_spi_bridge_lite)
        target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
        target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
    endif()
elseif(CONFIG_AS_INIC_AP)
    if(CONFIG_WHC_INTF_SPI)
        list(
            APPEND CSRC
            ${HOSTDIR}/spi/whc_spi_host.c
            ${HOSTDIR}/spi/whc_spi_host_trx.c
            ${HOSTDIR}/whc_host_api.c
            ${HOSTDIR}/whc_host_api_ext.c
            ${HOSTDIR}/whc_host_api_basic.c
            ${HOSTDIR}/whc_host_cust_evt.c
        )
        ameba_internal_library(wifi_whc_spi_host)
        target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
        target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
    elseif(NOT CONFIG_AMEBA_RLS)
        list(
            APPEND CSRC
            ${HOSTDIR}/ipc/whc_ipc_host.c 
            ${HOSTDIR}/ipc/whc_ipc_host_trx.c 
            ${HOSTDIR}/ipc/whc_ipc_host_api.c 
            ${HOSTDIR}/ipc/whc_ipc_host_api_basic.c 
            ${HOSTDIR}/ipc/whc_ipc_host_api_ext.c 
            ${COMDIR}/whc_ipc_msg_queue.c
            )

        set(TARGET_LIB whc)

        if(CONFIG_MP_INCLUDED)
            if(CONFIG_MP_SHRINK)
                set(TARGET_LIB wifi_whc_ap_mp_shrink)
            else()
                set(TARGET_LIB wifi_whc_ap_mp)
            endif()
        else()
            set(TARGET_LIB wifi_whc_ap)
        endif()

        ameba_app_library(${TARGET_LIB})
        target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
        target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
    endif()
endif()
