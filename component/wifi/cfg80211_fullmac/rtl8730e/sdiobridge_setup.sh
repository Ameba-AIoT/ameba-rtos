#!/bin/bash

sed -i 's/CONFIG_FULLMAC_HCI_IPC = y/CONFIG_FULLMAC_HCI_IPC = n/g' Makefile
sed -i 's/CONFIG_FULLMAC_HCI_SDIO = n/CONFIG_FULLMAC_HCI_SDIO = y/g' Makefile
sed -i 's/CONFIG_SDIO_BRIDGE = n/CONFIG_SDIO_BRIDGE = y/g' Makefile

mkdir -p include
cp ../../inic/inic_def.h include
cp ../../inic/common/inic_dev.h include
cp ../../driver/include/rom_rtw_defs.h include
cp ../../driver/include/rtw_wifi_defs.h include
cp ../../api/wifi_intf_drv_to_app_basic.h include
cp ../../../soc/amebadplus/fwlib/include/ameba_inic.h include
cp ../../../soc/amebadplus/usrcfg/ameba_wificfg.c sdio

echo "SDIO setup complete"
