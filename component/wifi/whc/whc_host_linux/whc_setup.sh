#!/bin/bash

mkdir -p ./common/include

# --- Helper functions ---
ac_set()   { sed -i "s/#undef $1/#define $1 1/g" ./common/autoconf.h; }
ac_unset() { sed -i "s/#define $1 1/#undef $1/g" ./common/autoconf.h; }
mf_set()   { sed -i "s/$1 = n/$1 = y/g" Makefile; }
mf_unset() { sed -i "s/$1 = y/$1 = n/g" Makefile; }

INTF="$1"
# Validate interface
case "$INTF" in
	sdio|spi|usb) ;;
	*) echo "Error: invalid interface '$INTF', must be one of: sdio, spi, usb"; exit 1 ;;
esac

# --- Step 1: Disable IPC, will configure host-only mode ---
mf_unset CONFIG_WHC_HCI_IPC

# --- Step 2: Select host mode ---
echo "choose target host mode:"
echo "  1) WPAoH host with cfg80211"
echo "  2) WPAoD host without cfg80211"
read -p "choose target host mode: " choice

case $choice in
	1) echo "WPAoH host with cfg80211S select"; mf_set CONFIG_WHC_WIFI_API_PATH ;;
	2) echo "WPAoD host without cfg80211 select"; mf_unset CONFIG_WHC_WIFI_API_PATH ;;
	*) echo "Invalid selection" ;;
esac

# --- Step 3: Configure interface, features, and target IC ---
# Reset all configs
mf_unset CONFIG_WHC_HCI_SDIO
mf_unset CONFIG_WHC_HCI_SPI
mf_unset CONFIG_WHC_HCI_USB
mf_unset CONFIG_NAN
mf_unset CONFIG_P2P
mf_unset CONFIG_BT_INIC
mf_unset CONFIG_RMESH
mf_unset CONFIG_FW_DOWNLOAD
mf_unset CONFIG_MP_FIRMWARE

# Safely shift arguments, suppress errors, never fail
shift 2>/dev/null || true

# 3a. Enable extra features (nan, p2p, bt, rmesh)
for feat in "$@"; do
	case "$feat" in
		nan)   mf_set CONFIG_NAN ;;
		p2p)   mf_set CONFIG_P2P ;;
		bt)
			mf_set CONFIG_BT_INIC
			if [ "$INTF" == "spi" ]; then
				cp ../../bluetooth/example/bt_host/linux_driver/rtb_spi.c ./common/spi
				cp ../../bluetooth/example/bt_host/linux_driver/rtb_spi.h ./common/spi
			elif [ "$INTF" == "sdio" ]; then
				cp ../../../bluetooth/example/bt_host/linux_driver/sdio_dplus/rtb_sdio.c ./common/sdio
				cp ../../../bluetooth/example/bt_host/linux_driver/sdio_dplus/rtb_sdio.h ./common/sdio
			fi
			;;
		rmesh) mf_set CONFIG_RMESH ;;
	esac
done

# 3b. Select target IC
ac_unset CONFIG_AMEBADPLUS
ac_unset CONFIG_AMEBALITE
ac_unset CONFIG_AMEBAGREEN2
ac_unset CALCULATE_FREE_TXBD
ac_unset GREEN2_WA

echo "choose target IC:"
case "$INTF" in
	sdio) mf_set CONFIG_WHC_HCI_SDIO ; echo "  1) AMEBADPLUS"; echo "  2) AMEBAGREEN2"; echo "  3) AMEBAGREEN2 & FW_DOWNLOAD"; echo "  4) AMEBAX" ;;
	spi)  mf_set CONFIG_WHC_HCI_SPI  ; echo "  1) AMEBADPLUS"; echo "  2) AMEBAGREEN2"; echo "  3) AMEBALITE" ;;
	usb)  mf_set CONFIG_WHC_HCI_USB  ; echo "  1) AMEBADPLUS"; echo "  2) AMEBAGREEN2"; echo "  3) AMEBAGREEN2 & FW_DOWNLOAD" ;;
esac

read -p "choose target IC: " choice

case "$INTF:$choice" in
	# --- sdio ---
	sdio:1)
		echo "AMEBADPLUS select"
		ac_set CONFIG_AMEBADPLUS; ac_set CALCULATE_FREE_TXBD
		cp ../../../soc/amebadplus/fwlib/include/ameba_inic.h ./common/include
		;;
	sdio:2)
		echo "AMEBAGREEN2 select"
		ac_set CONFIG_AMEBAGREEN2; ac_set GREEN2_WA
		cp ../../../soc/amebagreen2/fwlib/include/ameba_inic.h ./common/include
		;;
	sdio:3)
		echo "AMEBAGREEN2 with FW DOWNLOAD select"
		ac_set CONFIG_AMEBAGREEN2; ac_set GREEN2_WA
		mf_set CONFIG_FW_DOWNLOAD
		cp ../../../soc/amebagreen2/fwlib/include/ameba_inic.h ./common/include
		;;
	sdio:4)
		echo "AMEBAX select"
		ac_set CONFIG_AMEBAGREEN2; ac_set GREEN2_WA
		cp ../../../soc/RTL8720F/fwlib/include/ameba_inic.h ./common/include
		;;
	# --- spi ---
	spi:1)
		echo "AMEBADPLUS select"
		ac_set CONFIG_AMEBADPLUS; ac_set CALCULATE_FREE_TXBD
		;;
	spi:2)
		echo "AMEBAGREEN2 select"
		ac_set CONFIG_AMEBAGREEN2
		;;
	spi:3)
		echo "AMEBALITE select"
		ac_set CONFIG_AMEBALITE
		;;
	# --- usb ---
	usb:1)
		echo "AMEBADPLUS select"
		ac_set CONFIG_AMEBADPLUS
		;;
	usb:2)
		echo "AMEBAGREEN2 select"
		ac_set CONFIG_AMEBAGREEN2
		;;
	usb:3)
		echo "AMEBAGREEN2 with FW DOWNLOAD select"
		ac_set CONFIG_AMEBAGREEN2
		mf_set CONFIG_FW_DOWNLOAD
		;;
	*)
		echo "Invalid selection"
		;;
esac

# --- Step 4: MP firmware selection (only when FW_DOWNLOAD is enabled) ---
if grep -q "CONFIG_FW_DOWNLOAD = y" Makefile; then
	read -p "Use MP firmware? (y/N): " mp_choice
	case "$mp_choice" in
		y|Y) echo "MP firmware select"; mf_set CONFIG_MP_FIRMWARE ;;
		*)   echo "Normal firmware select" ;;
	esac
fi

# --- Step 5: Copy common headers ---
cp ../../common/rtw_wifi_common.h ./common/include
cp ../../common/rtw_inic_common.h ./common/include
cp ../whc_def.h ./common/include
cp ../whc_dev/whc_dev.h ./common/include
cp ../whc_dev/whc_dev_common.h ./common/include
cp ../whc_dev/whc_dev_intf.h ./common/include
cp ../whc_dev/whc_dev_struct.h ./common/include
cp ../whc_dev/whc_dev_protocal_offload.h ./common/include
cp ../../api/wifi_api_types.h ./common/include
cp ../../api/wifi_api_event.h ./common/include
cp ../../driver/intf/wifi_intf_drv_to_app_internal.h ./common/include
cp ../../../soc/usrcfg/common/ameba_wificfg_common.h ./common/include
cp ../../../bluetooth/driver/bt_inic/bt_inic_defs.h ./common/include
cp ../whc_dev/whc_dev_tcpip.h ./common/include
cp ../whc_dev/whc_dev_powersave.h ./common/include

echo "WHC setup complete"
