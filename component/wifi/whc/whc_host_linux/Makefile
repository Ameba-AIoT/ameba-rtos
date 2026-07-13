# SPDX-License-Identifier: GPL-2.0
export CONFIG_AMEBADPLUS = n
export CONFIG_AMEBAGREEN2 = n
export CONFIG_AMEBALITE = n
export CONFIG_AMEBAX = n
export CALCULATE_FREE_TXBD = n
export CONFIG_WHC_HCI_IPC = y
export CONFIG_WHC_HCI_SDIO = n
export CONFIG_WHC_HCI_SPI = n
export CONFIG_WHC_HCI_USB = n
export CONFIG_NAN = n
export CONFIG_P2P = n
export CONFIG_MCC = n
export CONFIG_BT_INIC = n
export CONFIG_IEEE80211R = n
export CONFIG_SUPPLICANT_SME = n
export CONFIG_WHC_CMD_PATH = y
export CONFIG_WHC_WIFI_API_PATH = y
export CONFIG_WHC_HOST_RTW_CLI = n
export CONFIG_WHC_ACTIVE_DETECT = n
export CONFIG_RMESH = n
export CONFIG_FW_DOWNLOAD = n
export CONFIG_WHCH = n

ifeq ($(CONFIG_WHC_HCI_IPC),y)
# ipc build
include $(src)/driver/ipc/Kbuild
else
# non-ipc build
KDIR ?= /lib/modules/$(shell uname -r)/build

M := $(CURDIR)
B := $(abspath $O/$M)

ifeq ($(CONFIG_WHC_HCI_SPI),y)
SUB := driver/spi
else ifeq ($(CONFIG_WHC_HCI_USB),y)
SUB := driver/usb
else ifeq ($(CONFIG_WHC_HCI_SDIO),y)
SUB := driver/sdio
endif

default:
	$(MAKE) -C $(KDIR) M=$(M)/$(SUB)
modules modules_install clean help kernelrelease:
	$(MAKE) -C $(KDIR) M=$(M) $@
	$(MAKE) -C $(KDIR) M=$(M)/$(SUB) $@

endif
