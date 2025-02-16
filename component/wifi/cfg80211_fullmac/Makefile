# SPDX-License-Identifier: GPL-2.0
export CONFIG_NAN = n
export CONFIG_P2P = n
export CONFIG_MCC = n
export CONFIG_BT_INIC = n
export CONFIG_FULLMAC_HCI_IPC = y
export CONFIG_FULLMAC_HCI_SDIO = n
export CONFIG_FULLMAC_HCI_SPI = n
export CONFIG_FULLMAC_HCI_USB = n
export CONFIG_FULLMAC_BRIDGE = n

ifeq ($(CONFIG_FULLMAC_HCI_IPC),y)

ccflags-$(CONFIG_NAN) += -DCONFIG_NAN
ccflags-$(CONFIG_P2P) += -DCONFIG_P2P
ccflags-$(CONFIG_FULLMAC_HCI_IPC) += -DCONFIG_FULLMAC
ccflags-$(CONFIG_FULLMAC_HCI_IPC) += -I$(srctree)/../../firmware/component/wifi/driver/include
ccflags-$(CONFIG_FULLMAC_HCI_IPC) += -I$(srctree)/../../firmware/component/wifi/api
ccflags-$(CONFIG_FULLMAC_HCI_IPC) += -I$(srctree)/../../firmware/component/wifi/whc
ccflags-$(CONFIG_FULLMAC_HCI_IPC) += -I$(srctree)/../../firmware/component/wifi/whc/whc_dev
ccflags-$(CONFIG_FULLMAC_HCI_IPC) += -I$(srctree)/$(src)/fullmac/ipc
ccflags-$(CONFIG_FULLMAC_HCI_IPC) += -I$(srctree)/$(src)/common/netdev
ccflags-$(CONFIG_FULLMAC_HCI_IPC) += -I$(srctree)/$(src)/fullmac

ccflags-$(CONFIG_FULLMAC_HCI_IPC) += -DCONFIG_FULLMAC_HCI_IPC

obj-$(CONFIG_WIFI_RTK_CFG80211_FULLMAC)	+= fullmac-ipc.o
fullmac-ipc-objs			:= ./common/netdev/rtw_drv_probe.o \
					./fullmac/rtw_wiphy.o \
					./fullmac/rtw_regd.o \
					./common/netdev/rtw_ethtool_ops.o \
					./common/netdev/rtw_netdev_ops.o \
					./fullmac/rtw_netdev_ops_p2p.o \
					./fullmac/rtw_cfgvendor.o \
					./fullmac/rtw_proc.o \
					./fullmac/rtw_cfg80211_ops.o \
					./fullmac/rtw_cfg80211_ops_ap.o \
					./fullmac/rtw_cfg80211_ops_key.o \
					./fullmac/rtw_cfg80211_ops_nan.o \
					./fullmac/rtw_cfg80211_ops_p2p.o \
					./fullmac/rtw_acs.o \
					./fullmac/rtw_promisc.o \
					./fullmac/ipc/rtw_llhw_hci.o \
					./fullmac/ipc/rtw_llhw_ops.o \
					./fullmac/ipc/rtw_llhw_event.o \
					./fullmac/ipc/rtw_llhw_recv.o \
					./fullmac/ipc/rtw_llhw_xmit.o \
					./fullmac/ipc/rtw_llhw_msg.o \
					./fullmac/ipc/rtw_llhw_mem.o \
					./fullmac/ipc/ameba_wificfg.o


else

KDIR ?= /lib/modules/$(shell uname -r)/build

# Regular build, build here.
M := $(CURDIR)
B := $(abspath $O/$M)

ifeq ($(CONFIG_FULLMAC_BRIDGE),y)

#fullmac bridge
ifeq ($(CONFIG_FULLMAC_HCI_SDIO),y)
SUB := bridge/sdio_bridge
endif

else

#fullmac
ifeq ($(CONFIG_FULLMAC_HCI_SPI),y)
SUB := fullmac/spi
else ifeq ($(CONFIG_FULLMAC_HCI_USB),y)
SUB := fullmac/usb
else ifeq ($(CONFIG_FULLMAC_HCI_SDIO),y)
SUB := fullmac/sdio
endif

endif

default:
	$(MAKE) -C $(KDIR) M=$(M)/$(SUB)
modules modules_install clean help kernelrelease:
	$(MAKE) -C $(KDIR) M=$(M) $@
	$(MAKE) -C $(KDIR) M=$(M)/$(SUB) $@

endif

