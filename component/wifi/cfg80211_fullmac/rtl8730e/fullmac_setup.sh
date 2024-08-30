#!/bin/bash

sed -i 's/CONFIG_FULLMAC_HCI_IPC = y/CONFIG_FULLMAC_HCI_IPC = n/g' Makefile

if [ "$1" == "spi" ]; then
	sed -i 's/CONFIG_FULLMAC_HCI_SDIO = y/CONFIG_FULLMAC_HCI_SDIO = n/g' Makefile
	sed -i 's/CONFIG_FULLMAC_HCI_USB = y/CONFIG_FULLMAC_HCI_USB = n/g' Makefile
	sed -i 's/CONFIG_FULLMAC_HCI_SPI = n/CONFIG_FULLMAC_HCI_SPI = y/g' Makefile
elif [ "$1" == "usb" ]; then
	sed -i 's/CONFIG_FULLMAC_HCI_SDIO = y/CONFIG_FULLMAC_HCI_SDIO = n/g' Makefile
	sed -i 's/CONFIG_FULLMAC_HCI_SPI = y/CONFIG_FULLMAC_HCI_SPI = n/g' Makefile
	sed -i 's/CONFIG_FULLMAC_HCI_USB = n/CONFIG_FULLMAC_HCI_USB = y/g' Makefile
else
	sed -i 's/CONFIG_FULLMAC_HCI_SDIO = n/CONFIG_FULLMAC_HCI_SDIO = y/g' Makefile
	sed -i 's/CONFIG_FULLMAC_HCI_SPI = y/CONFIG_FULLMAC_HCI_SPI = n/g' Makefile
	sed -i 's/CONFIG_FULLMAC_HCI_USB = y/CONFIG_FULLMAC_HCI_USB = n/g' Makefile
fi

if [ "$#" -ge 2 ]; then
	if [ "$2" == "nan" ]; then
		sed -i 's/CONFIG_NAN = n/CONFIG_NAN = y/g' Makefile
	fi
fi

mkdir -p include
cp ../../inic/inic_def.h include
cp ../../inic/common/inic_dev.h include
cp ../../inic/common/inic_dev_protocal_offload.h include
cp ../../driver/include/rom_rtw_defs.h include
cp ../../driver/include/rtw_wifi_defs.h include
cp ../../api/wifi_intf_drv_to_app_basic.h include
cp ../../../soc/amebadplus/fwlib/include/ameba_inic.h include
cp ../../../soc/amebadplus/usrcfg/ameba_wificfg.c sdio

echo "Fullmac setup complete"
