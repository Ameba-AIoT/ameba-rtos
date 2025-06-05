# SPDX-License-Identifier: GPL-2.0
export CONFIG_NAN = n
export CONFIG_P2P = n
export CONFIG_MCC = n
export CONFIG_BT_INIC = n
export CONFIG_FULLMAC_HCI_IPC = y
export CONFIG_FULLMAC_HCI_SDIO = n
export CONFIG_FULLMAC_HCI_SPI = n
export CONFIG_FULLMAC_HCI_USB = n
export CONFIG_WHC_BRIDGE = n
export CONFIG_WHC_BRIDGE_WPA_OPS = n

ifeq ($(CONFIG_FULLMAC_HCI_IPC),y)

ccflags-$(CONFIG_NAN) += -DCONFIG_NAN
ccflags-$(CONFIG_P2P) += -DCONFIG_P2P
ccflags-$(CONFIG_FULLMAC_HCI_IPC) += -DCONFIG_FULLMAC
ccflags-$(CONFIG_FULLMAC_HCI_IPC) += -I$(srctree)/../../firmware/component/wifi/driver/include
ccflags-$(CONFIG_FULLMAC_HCI_IPC) += -I$(srctree)/../../firmware/component/wifi/driver/intf
ccflags-$(CONFIG_FULLMAC_HCI_IPC) += -I$(srctree)/../../firmware/component/wifi/api
ccflags-$(CONFIG_FULLMAC_HCI_IPC) += -I$(srctree)/../../firmware/component/wifi/whc
ccflags-$(CONFIG_FULLMAC_HCI_IPC) += -I$(srctree)/../../firmware/component/wifi/whc/whc_dev
ccflags-$(CONFIG_FULLMAC_HCI_IPC) += -I$(srctree)/$(src)/fullmac/ipc
ccflags-$(CONFIG_FULLMAC_HCI_IPC) += -I$(srctree)/$(src)/common/netdev
ccflags-$(CONFIG_FULLMAC_HCI_IPC) += -I$(srctree)/$(src)/fullmac

ccflags-$(CONFIG_FULLMAC_HCI_IPC) += -DCONFIG_FULLMAC_HCI_IPC

obj-$(CONFIG_WIFI_RTK_CFG80211_FULLMAC)	+= fullmac-ipc.o
fullmac-ipc-objs			:= ./common/netdev/whc_host_drv_probe.o \
					./fullmac/whc_fullmac_host_wiphy.o \
					./fullmac/whc_fullmac_host_regd.o \
					./common/netdev/whc_host_ethtool_ops.o \
					./common/netdev/whc_host_netdev_ops.o \
					./fullmac/whc_fullmac_host_netdev_ops_p2p.o \
					./fullmac/whc_fullmac_host_cfgvendor.o \
					./fullmac/whc_fullmac_host_proc.o \
					./fullmac/whc_fullmac_host_ops.o \
					./fullmac/whc_fullmac_host_ops_ap.o \
					./fullmac/whc_fullmac_host_ops_key.o \
					./fullmac/whc_fullmac_host_ops_nan.o \
					./fullmac/whc_fullmac_host_ops_p2p.o \
					./fullmac/whc_fullmac_host_acs.o \
					./fullmac/whc_fullmac_host_promisc.o \
					./fullmac/ipc/whc_fullmac_ipc_host_hci.o \
					./fullmac/ipc/whc_fullmac_ipc_host_ops.o \
					./fullmac/ipc/whc_fullmac_ipc_host_event.o \
					./fullmac/ipc/whc_fullmac_ipc_host_recv.o \
					./fullmac/ipc/whc_fullmac_ipc_host_xmit.o \
					./fullmac/ipc/whc_fullmac_ipc_host_msg.o \
					./fullmac/ipc/whc_fullmac_ipc_host_mem.o \
					./fullmac/ipc/ameba_wificfg.o


else

KDIR ?= /lib/modules/$(shell uname -r)/build

# Regular build, build here.
M := $(CURDIR)
B := $(abspath $O/$M)

ifeq ($(CONFIG_WHC_BRIDGE),y)

#fullmac bridge
ifeq ($(CONFIG_FULLMAC_HCI_SDIO),y)
SUB := bridge/sdio_bridge
else ifeq ($(CONFIG_FULLMAC_HCI_SPI),y)
SUB := bridge/spi_bridge
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

