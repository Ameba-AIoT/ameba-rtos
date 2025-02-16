#!/bin/bash

sed -i 's/CONFIG_FULLMAC_HCI_IPC = y/CONFIG_FULLMAC_HCI_IPC = n/g' Makefile
sed -i 's/CONFIG_FULLMAC_BRIDGE = y/CONFIG_FULLMAC_BRIDGE = n/g' Makefile
sed -i 's/CONFIG_FULLMAC_BRIDGE_LITE = y/CONFIG_FULLMAC_BRIDGE_LITE = n/g' Makefile

if [ "$1" == "spi" ]; then
	sed -i 's/CONFIG_FULLMAC_HCI_SDIO = y/CONFIG_FULLMAC_HCI_SDIO = n/g' Makefile
	sed -i 's/CONFIG_FULLMAC_HCI_USB = y/CONFIG_FULLMAC_HCI_USB = n/g' Makefile
	sed -i 's/CONFIG_FULLMAC_HCI_SPI = n/CONFIG_FULLMAC_HCI_SPI = y/g' Makefile
elif [ "$1" == "usb" ]; then
	sed -i 's/CONFIG_FULLMAC_HCI_SDIO = y/CONFIG_FULLMAC_HCI_SDIO = n/g' Makefile
	sed -i 's/CONFIG_FULLMAC_HCI_SPI = y/CONFIG_FULLMAC_HCI_SPI = n/g' Makefile
	sed -i 's/CONFIG_FULLMAC_HCI_USB = n/CONFIG_FULLMAC_HCI_USB = y/g' Makefile
elif [ "$1" == "sdio" ]; then
	sed -i 's/CONFIG_FULLMAC_HCI_SDIO = n/CONFIG_FULLMAC_HCI_SDIO = y/g' Makefile
	sed -i 's/CONFIG_FULLMAC_HCI_SPI = y/CONFIG_FULLMAC_HCI_SPI = n/g' Makefile
	sed -i 's/CONFIG_FULLMAC_HCI_USB = y/CONFIG_FULLMAC_HCI_USB = n/g' Makefile
fi

if [ "$#" -ge 2 ]; then
	for ((i=2;i<=$#;i++))
	do
		eval j=\$$i
		if [ "$j" == "bridge" ]; then
			sed -i 's/CONFIG_FULLMAC_BRIDGE = n/CONFIG_FULLMAC_BRIDGE = y/g' Makefile
		fi
	done
fi

if [ "$#" -ge 2 ]; then
	for ((i=2;i<=$#;i++))
	do
		eval j=\$$i
		if [ "$j" == "nan" ]; then
			sed -i 's/CONFIG_NAN = n/CONFIG_NAN = y/g' Makefile
		elif [ "$j" == "bt" ]; then
			if [ "$1" == "spi" ]; then
				sed -i 's/CONFIG_BT_INIC = n/CONFIG_BT_INIC = y/g' Makefile
				cp ../../bluetooth/example/bt_host/linux_driver/rtb_spi.c spi
				cp ../../bluetooth/example/bt_host/linux_driver/rtb_spi.h spi
			fi
		fi
	done
fi

if [[ "$1" == "sdio" ]]; then
	sed -i 's/#define CONFIG_AMEBADPLUS 1/#undef CONFIG_AMEBADPLUS/g' ./common/autoconf.h
	sed -i 's/#define CONFIG_AMEBAGREEN2 1/#undef CONFIG_AMEBAGREEN2/g' ./common/autoconf.h
	sed -i 's/#define CONFIG_FW_DOWNLOAD 1/#undef CONFIG_FW_DOWNLOAD/g' ./common/autoconf.h
	sed -i 's/#define CALCULATE_FREE_TXBD 1/#undef CALCULATE_FREE_TXBD/g' ./common/autoconf.h

	echo "choose target IC:"
	echo "1) AMEBADPLUS"
	echo "2) AMEBAGREEN2"
	echo "3) AMEBAGREEN2 & FW_DOWNLOAD"

	read -p "choose target IC: " choice
    case $choice in
        1)
            echo "AMEBADPLUS select"
            sed -i 's/#undef CONFIG_AMEBADPLUS/#define CONFIG_AMEBADPLUS 1/g' ./common/autoconf.h
            sed -i 's/#undef CALCULATE_FREE_TXBD/#define CALCULATE_FREE_TXBD 1/g' ./common/autoconf.h
            ;;
        2)
            echo "AMEBAGREEN2 select"
            sed -i 's/#undef CONFIG_AMEBAGREEN2/#define CONFIG_AMEBAGREEN2 1/g' ./common/autoconf.h
            ;;
        3)
            echo "AMEBAGREEN2 with FW DOWNLOAD select"
            sed -i 's/#undef CONFIG_AMEBAGREEN2/#define CONFIG_AMEBAGREEN2 1/g' ./common/autoconf.h
            sed -i 's/#undef CONFIG_FW_DOWNLOAD/#define CONFIG_FW_DOWNLOAD 1/g' ./common/autoconf.h
            ;;
        *)
            echo "Invalid selection"
            ;;
    esac
fi

if [[ "$1" == "usb" ]]; then
	sed -i 's/#define CONFIG_AMEBADPLUS 1/#undef CONFIG_AMEBADPLUS/g' ./common/autoconf.h
	sed -i 's/#define CONFIG_AMEBAGREEN2 1/#undef CONFIG_AMEBAGREEN2/g' ./common/autoconf.h
	sed -i 's/#define CONFIG_FW_DOWNLOAD 1/#undef CONFIG_FW_DOWNLOAD/g' ./common/autoconf.h
	sed -i 's/#define CALCULATE_FREE_TXBD 1/#undef CALCULATE_FREE_TXBD/g' ./common/autoconf.h

	echo "choose target IC:"
	echo "1) AMEBADPLUS"
	echo "2) AMEBAGREEN2"

	read -p "choose target IC: " choice
    case $choice in
        1)
            echo "AMEBADPLUS select"
            sed -i 's/#undef CONFIG_AMEBADPLUS/#define CONFIG_AMEBADPLUS 1/g' ./common/autoconf.h
            ;;
        2)
            echo "AMEBAGREEN2 select"
            sed -i 's/#undef CONFIG_AMEBAGREEN2/#define CONFIG_AMEBAGREEN2 1/g' ./common/autoconf.h
            ;;
        *)
            echo "Invalid selection"
            ;;
    esac
fi

mkdir -p ./common/include
cp ../common/rtw_wifi_common.h ./common/include
cp ../common/rtw_inic_common.h ./common/include
cp ../whc/whc_def.h ./common/include
cp ../whc/whc_dev/whc_dev.h ./common/include
cp ../whc/whc_dev/whc_dev_struct.h ./common/include
cp ../whc/whc_dev/whc_dev_protocal_offload.h ./common/include
cp ../driver/include/rom_rtw_defs.h ./common/include
cp ../driver/include/rtw_wifi_defs.h ./common/include
cp ../api/wifi_intf_drv_to_app_basic.h ./common/include
cp ../api/wifi_ind.h ./common/include
cp ../../soc/amebadplus/fwlib/include/ameba_inic.h ./common/include
cp ../../soc/amebadplus/usrcfg/ameba_wificfg.c ./common/sdio
cp ../../bluetooth/driver/bt_inic/bt_inic_defs.h ./common/include

echo "Fullmac setup complete"
