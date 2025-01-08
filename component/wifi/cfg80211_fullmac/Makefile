# SPDX-License-Identifier: GPL-2.0
export CONFIG_NAN = n
export CONFIG_P2P = n
export CONFIG_MCC = n
export CONFIG_BT_INIC = n
export CONFIG_FULLMAC_HCI_IPC = y
export CONFIG_FULLMAC_HCI_SDIO = n
export CONFIG_FULLMAC_HCI_SPI = n
export CONFIG_FULLMAC_HCI_USB = n
export CONFIG_SDIO_BRIDGE = n


ifeq ($(CONFIG_FULLMAC_HCI_IPC),y)

ccflags-$(CONFIG_NAN) += -DCONFIG_NAN
ccflags-$(CONFIG_P2P) += -DCONFIG_P2P
ccflags-$(CONFIG_FULLMAC_HCI_IPC) += -DCONFIG_FULLMAC
ccflags-$(CONFIG_FULLMAC_HCI_IPC) += -I$(srctree)/../../firmware/component/wifi/driver/include
ccflags-$(CONFIG_FULLMAC_HCI_IPC) += -I$(srctree)/../../firmware/component/wifi/api
ccflags-$(CONFIG_FULLMAC_HCI_IPC) += -I$(srctree)/../../firmware/component/wifi/inic

ccflags-$(CONFIG_FULLMAC_HCI_IPC) += -I$(srctree)/$(src)/ipc -I$(srctree)/../../firmware/component/wifi/inic/ipc -DCONFIG_FULLMAC_HCI_IPC

obj-$(CONFIG_WIFI_RTK_CFG80211_FULLMAC)	+= fullmac-ipc.o
fullmac-ipc-objs			:= rtw_drv_probe.o \
					rtw_wiphy.o \
					rtw_regd.o \
					rtw_ethtool_ops.o \
					rtw_netdev_ops.o \
					rtw_netdev_ops_p2p.o \
					rtw_cfgvendor.o \
					rtw_proc.o \
					rtw_cfg80211_ops.o \
					rtw_cfg80211_ops_ap.o \
					rtw_cfg80211_ops_key.o \
					rtw_cfg80211_ops_nan.o \
					rtw_cfg80211_ops_p2p.o \
					rtw_acs.o \
					rtw_promisc.o \
					ipc/rtw_llhw_hci.o \
					ipc/rtw_llhw_ops.o \
					ipc/rtw_llhw_event.o \
					ipc/rtw_llhw_recv.o \
					ipc/rtw_llhw_xmit.o \
					ipc/rtw_llhw_msg.o \
					ipc/rtw_llhw_mem.o \
					ipc/ameba_wificfg.o


else

KDIR ?= /lib/modules/$(shell uname -r)/build

# Regular build, build here.
M := $(CURDIR)
B := $(abspath $O/$M)

ifeq ($(CONFIG_FULLMAC_HCI_SPI),y)
SUB := spi
else ifeq ($(CONFIG_FULLMAC_HCI_USB),y)
SUB := usb
else ifneq ($(CONFIG_SDIO_BRIDGE),y)
SUB := sdio
else
SUB := sdio_bridge
endif

default:
	$(MAKE) -C $(KDIR) M=$(M)/$(SUB)
modules modules_install clean help kernelrelease:
	$(MAKE) -C $(KDIR) M=$(M) $@
	$(MAKE) -C $(KDIR) M=$(M)/$(SUB) $@

endif

