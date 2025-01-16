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

set(INICDRV ${BASEDIR}/component/wifi/inic)
set(COMDRV  ${BASEDIR}/component/wifi/inic/none_ipc_dev)
set(INICCOM ${BASEDIR}/component/wifi/inic/none_ipc_host)
set(SDIODRV ${BASEDIR}/component/wifi/inic/sdio)
set(USBDRV  ${BASEDIR}/component/wifi/inic/usb)
set(SPIDRV  ${BASEDIR}/component/wifi/inic/spi)

if(CONFIG_INIC_INTF_USB)
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

if(CONFIG_AS_INIC_NP)
    if(CONFIG_INIC_INTF_SPI)
        set(
            CSRC 
            ${INICDRV}/spi/inic_spi_dev.c
            ${COMDRV}/inic_dev_msg_queue.c
            ${COMDRV}/inic_dev_trx.c
            ${COMDRV}/inic_dev_api.c
            ${COMDRV}/inic_dev_protocal_offload.c
            ${COMDRV}/inic_dev_cust_evt.c
        )
        ameba_internal_library(wifi_inic_spi_dev)
        target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
        target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
    elseif(CONFIG_INIC_INTF_SDIO)
        set(
            CSRC
            ${COMDRV}/inic_dev_msg_queue.c
            ${COMDRV}/inic_dev_trx.c
            ${COMDRV}/inic_dev_api.c
            ${COMDRV}/inic_dev_protocal_offload.c
            ${COMDRV}/inic_dev_cust_evt.c
            ${SDIODRV}/inic_sdio_dev.c
        )
        ameba_internal_library(wifi_inic_sdio_dev)
        target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
        target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
    elseif(CONFIG_INIC_INTF_USB)
        set(
            CSRC
            ${COMDRV}/inic_dev_msg_queue.c
            ${COMDRV}/inic_dev_trx.c
            ${COMDRV}/inic_dev_api.c
            ${COMDRV}/inic_dev_protocal_offload.c
            ${USBDRV}/inic_usb_dev.c
        )
        ameba_internal_library(wifi_inic_usb_dev)
        target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
        target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
    endif()
elseif(CONFIG_FULLMAC_BRIDGE)
    if(CONFIG_SDIO_BRIDGE)
        set(
            CSRC
            ${COMDRV}/inic_dev_msg_queue.c
            ${COMDRV}/inic_dev_trx.c
            ${COMDRV}/inic_dev_api.c
            ${SDIODRV}/inic_sdio_dev.c
            ${COMDRV}/inic_dev_bridge.c
            ${COMDRV}/inic_dev_cust_evt.c
        )
        ameba_internal_library(wifi_fullmac_sdio_bridge)
        target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
        target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
    elseif(CONFIG_SPI_BRIDGE)
        set(
            CSRC
            ${COMDRV}/inic_dev_msg_queue.c
            ${COMDRV}/inic_dev_trx.c
            ${COMDRV}/inic_dev_api.c
            ${COMDRV}/inic_dev_protocal_offload.c
            ${SPIDRV}/inic_spi_dev.c
            ${COMDRV}/inic_dev_bridge.c
            ${COMDRV}/inic_dev_cust_evt.c
        )
        ameba_internal_library(wifi_fullmac_spi_bridge)
        target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
        target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
    endif()
elseif(CONFIG_AS_INIC_AP)
    if(CONFIG_INIC_INTF_SPI)
		set(
			CSRC 
			${INICDRV}/spi/inic_spi_host.c
			${INICDRV}/spi/inic_spi_host_trx.c
			${INICCOM}/inic_host_api.c
			${INICCOM}/inic_host_api_ext.c
			${INICCOM}/inic_host_api_basic.c
			${INICCOM}/inic_host_cust_evt.c
		)
        ameba_internal_library(wifi_inic_spi_host)
        target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
        target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
	elseif(NOT CONFIG_AMEBA_RLS)
		set(
			CSRC 
			${INICDRV}/ipc/inic_ipc_host.c 
			${INICDRV}/ipc/inic_ipc_host_trx.c 
			${INICDRV}/ipc/inic_ipc_host_api.c 
			${INICDRV}/ipc/inic_ipc_host_api_basic.c 
			${INICDRV}/ipc/inic_ipc_host_api_ext.c 
			${INICDRV}/ipc/inic_ipc_msg_queue.c
		)

        set(TARGET_LIB inic)

        if(CONFIG_MP_INCLUDED)
            if(CONFIG_MP_SHRINK)
                set(TARGET_LIB wifi_inic_ap_mp_shrink)
            else()
                set(TARGET_LIB wifi_inic_ap_mp)
            endif()
        else()
            set(TARGET_LIB wifi_inic_ap)
        endif()

        ameba_app_library(${TARGET_LIB})
        target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
        target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
    endif()
endif()
