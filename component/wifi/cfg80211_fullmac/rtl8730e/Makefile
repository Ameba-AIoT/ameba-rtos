# SPDX-License-Identifier: GPL-2.0
export CONFIG_NAN = n
export CONFIG_P2P = n
export CONFIG_FULLMAC_HCI_IPC = y
export CONFIG_FULLMAC_HCI_SDIO = n

ifeq ($(CONFIG_FULLMAC_HCI_IPC),y)

ccflags-$(CONFIG_NAN) += -DCONFIG_NAN
ccflags-$(CONFIG_FULLMAC_RTK_RTL8730E) += -DCONFIG_FULLMAC
ccflags-$(CONFIG_FULLMAC_RTK_RTL8730E) += -I$(srctree)/../../firmware/component/wifi/driver/include
ccflags-$(CONFIG_FULLMAC_RTK_RTL8730E) += -I$(srctree)/../../firmware/component/wifi/api
ccflags-$(CONFIG_FULLMAC_RTK_RTL8730E) += -I$(srctree)/../../firmware/component/wifi/inic

ccflags-$(CONFIG_FULLMAC_HCI_IPC) += -I$(srctree)/$(src)/ipc -I$(srctree)/../../firmware/component/wifi/inic/ipc -DCONFIG_FULLMAC_HCI_IPC

obj-$(CONFIG_FULLMAC_HCI_IPC)	+= ipc/
obj-$(CONFIG_FULLMAC_RTK_RTL8730E)	+= fullmac-8730e.o
fullmac-8730e-objs			:= rtw_drv_probe.o \
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

else ifeq ($(CONFIG_FULLMAC_HCI_SDIO),y)

KDIR ?= /lib/modules/$(shell uname -r)/build

# Regular build, build here.
M := $(CURDIR)
B := $(abspath $O/$M)

default:
	$(MAKE) -C $(KDIR) M=$(M)/sdio
modules modules_install clean help kernelrelease:
	$(MAKE) -C $(KDIR) M=$(M) $@
	$(MAKE) -C $(KDIR) M=$(M)/sdio $@

endif

