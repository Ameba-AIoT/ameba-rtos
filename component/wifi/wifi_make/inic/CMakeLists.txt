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

if(CONFIG_AS_INIC_NP)
    if(CONFIG_INIC_INTF_SPI)
        set(
            CSRC 
            ${INICDRV}/spi/inic_spi_dev.c 
        )

        ameba_internal_library(wifi_inic_spi_dev)
        target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
        target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
    endif()
elseif(CONFIG_AS_INIC_AP)
    if(CONFIG_SPI_FULLMAC_HOST)
		set(
			CSRC 
			${INICDRV}/spi/inic_spi_host.c
			${INICDRV}/spi/inic_spi_host_api.c
			${INICDRV}/spi/inic_spi_host_api_ext.c
			${INICDRV}/spi/inic_spi_host_api_basic.c
			${INICDRV}/spi/inic_spi_host_trx.c
		)
	else()
		set(
			CSRC 
			${INICDRV}/ipc/inic_ipc_host.c 
			${INICDRV}/ipc/inic_ipc_host_trx.c 
			${INICDRV}/ipc/inic_ipc_host_api.c 
			${INICDRV}/ipc/inic_ipc_host_api_basic.c 
			${INICDRV}/ipc/inic_ipc_host_api_ext.c 
			${INICDRV}/ipc/inic_ipc_msg_queue.c
		)
	endif()

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
