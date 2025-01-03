set(DIR ${BASEDIR}/component/bluetooth)

set(
    CSRC ${DIR}/driver/hci/hci_if_inic.c
)

if(CONFIG_BT_INIC_H4 OR CONFIG_BT_INICD_H4)
    list(APPEND CSRC ${DIR}/driver/bt_inic/bt_inic_h4.c)
elseif(CONFIG_BT_INIC_H5 OR CONFIG_BT_INICD_H5)
    list(APPEND CSRC ${DIR}/driver/bt_inic/bt_inic_h5.c)
elseif(CONFIG_BT_INICD_SPI OR (CONFIG_SPI_FULLMAC AND CONFIG_BT_INIC_SHARE))
    list(APPEND CSRC ${DIR}/driver/bt_inic/bt_inic_spi_dev.c)
elseif(CONFIG_BT_INICD_SDIO OR (CONFIG_SDIO_FULLMAC AND CONFIG_BT_INIC_SHARE))
    list(APPEND CSRC ${DIR}/driver/bt_inic/bt_inic_sdio_dplus.c)
endif()

target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
