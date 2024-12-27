set(DIR ${BASEDIR}/component/bluetooth)

set(
    CSRC ${DIR}/driver/hci/hci_if_inic.c
)

if(CONFIG_BT_INIC_H4)
    list(APPEND CSRC ${DIR}/driver/bt_inic/bt_inic_h4.c)
elseif(CONFIG_BT_INIC_H5)
    list(APPEND CSRC ${DIR}/driver/bt_inic/bt_inic_h5.c)
elseif(CONFIG_BT_INIC_SPI OR CONFIG_SPI_FULLMAC)
    list(APPEND CSRC ${DIR}/driver/bt_inic/bt_inic_spi_dev.c)
elseif(CONFIG_BT_INIC_SDIO OR CONFIG_SDIO_FULLMAC)
    list(APPEND CSRC ${DIR}/driver/bt_inic/bt_inic_sdio_dplus.c)
endif()

target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
