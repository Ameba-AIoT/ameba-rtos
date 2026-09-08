#!/bin/bash
#
# Stage firmware headers and BT host glue into ./driver/include, ./driver/spi
# and ./driver/sdio for the out-of-tree build. This performs a FIXED set of
# file copies that the Kbuild include paths expect; it does NOT depend on
# .config. What actually gets compiled is decided later by Kbuild from the
# selected config, so staging everything unconditionally keeps this step
# independent of configuration.
#
# Usage: whc_setup.sh

set -e

mkdir -p ./driver/include

# --- Common headers ---
cp ../../common/rtw_wifi_common.h ./driver/include
cp ../../common/rtw_inic_common.h ./driver/include
cp ../whc_common/whc_def.h ./driver/include
cp ../../api/wifi_api_types.h ./driver/include
cp ../../api/wifi_api_event.h ./driver/include
cp ../../driver/intf/wifi_intf_drv_to_app_internal.h ./driver/include
cp ../../../soc/usrcfg/common/ameba_wificfg_common.h ./driver/include
cp ../../../bluetooth/driver/bt_inic/bt_inic_defs.h ./driver/include

# --- Firmware header (ameba_inic.h) ---
# The INIC TX/RX descriptor layout and packet opcodes are identical across the
# SDIO ICs (amebadplus/amebagreen2/RTL8720F), so a single canonical copy works
# for any target -- no per-IC selection needed.
cp ../../../soc/amebagreen2/fwlib/include/ameba_inic.h ./driver/include

# --- Bluetooth host glue (both interfaces, unconditionally) ---
# Kbuild compiles only the one matching CONFIG_BT_INIC + the selected interface.
cp ../../../bluetooth/example/bt_host/linux_driver/spi/rtb_spi.c ./driver/spi
cp ../../../bluetooth/example/bt_host/linux_driver/spi/rtb_spi.h ./driver/spi
cp ../../../bluetooth/example/bt_host/linux_driver/sdio_dplus/rtb_sdio.c ./driver/sdio
cp ../../../bluetooth/example/bt_host/linux_driver/sdio_dplus/rtb_sdio.h ./driver/sdio

echo "WHC staging complete"
