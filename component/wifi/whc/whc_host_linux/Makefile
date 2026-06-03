# SPDX-License-Identifier: GPL-2.0
export CONFIG_NAN = n
export CONFIG_P2P = n
export CONFIG_MCC = n
export CONFIG_BT_INIC = n
export CONFIG_WHC_HCI_IPC = y
export CONFIG_WHC_HCI_SDIO = n
export CONFIG_WHC_HCI_SPI = n
export CONFIG_WHC_HCI_USB = n
export CONFIG_IEEE80211R = n
export CONFIG_SUPPLICANT_SME = n
export CONFIG_WHC_CMD_PATH = y
export CONFIG_WHC_WIFI_API_PATH = y
export CONFIG_WHC_HOST_RTW_CLI = n
export CONFIG_WHC_ACTIVE_DETECT = n
export CONFIG_RMESH = n
export CONFIG_FW_DOWNLOAD = n
export CONFIG_WHC_HAL_CHIP = rtl8721f
export CONFIG_WHCH = n

ifeq ($(CONFIG_WHC_HCI_IPC),y)

drv_src := $(patsubst $(srctree)/%,%,$(src))
ccflags-$(CONFIG_NAN) += -DCONFIG_NAN
ccflags-$(CONFIG_P2P) += -DCONFIG_P2P
ccflags-$(CONFIG_IEEE80211R) += -DCONFIG_IEEE80211R
ccflags-$(CONFIG_SUPPLICANT_SME) += -DCONFIG_SUPPLICANT_SME
ccflags-$(CONFIG_WHC_HCI_IPC) += -DCONFIG_FULLMAC
ccflags-$(CONFIG_WHC_HCI_IPC) += -I$(srctree)/../../firmware/component/wifi/driver/include
ccflags-$(CONFIG_WHC_HCI_IPC) += -I$(srctree)/../../firmware/component/wifi/driver/intf
ccflags-$(CONFIG_WHC_HCI_IPC) += -I$(srctree)/../../firmware/component/wifi/api
ccflags-$(CONFIG_WHC_HCI_IPC) += -I$(srctree)/../../firmware/component/wifi/whc
ccflags-$(CONFIG_WHC_HCI_IPC) += -I$(srctree)/../../firmware/component/wifi/whc/whc_dev
ccflags-$(CONFIG_WHC_HCI_IPC) += -I$(srctree)/$(drv_src)/driver/ipc
ccflags-$(CONFIG_WHC_HCI_IPC) += -I$(srctree)/$(drv_src)/driver
ccflags-$(CONFIG_WHC_HCI_IPC) += -I$(srctree)/$(drv_src)/driver/netdev

ccflags-$(CONFIG_WHC_HCI_IPC) += -DCONFIG_WHC_HCI_IPC
ccflags-$(CONFIG_WHC_WIFI_API_PATH) += -DCONFIG_WHC_WIFI_API_PATH

obj-$(CONFIG_WIFI_RTK_CFG80211_FULLMAC)	+= whc-ipc.o
whc-ipc-objs			:= ./driver/whc_host_wiphy.o \
					./driver/whc_host_regd.o \
					./driver/netdev/whc_host_ethtool_ops.o \
					./driver/netdev/whc_host_netdev_ops.o \
					./driver/whc_host_netdev_ops_p2p.o \
					./driver/whc_host_cfgvendor.o \
					./driver/whc_host_proc.o \
					./driver/whc_host_ops.o \
					./driver/whc_host_ops_ap.o \
					./driver/whc_host_ops_key.o \
					./driver/whc_host_ops_nan.o \
					./driver/whc_host_ops_p2p.o \
					./driver/whc_host_acs.o \
					./driver/whc_host_promisc.o \
					./driver/whc_host_ft.o \
					./driver/ipc/whc_ipc_host_probe.o \
					./driver/ipc/whc_ipc_host_hci.o \
					./driver/ipc/whc_ipc_host_ops.o \
					./driver/ipc/whc_ipc_host_event.o \
					./driver/ipc/whc_ipc_host_recv.o \
					./driver/ipc/whc_ipc_host_xmit.o \
					./driver/ipc/whc_ipc_host_msg.o \
					./driver/ipc/whc_ipc_host_mem.o \
					./driver/ipc/ameba_wificfg.o

ifeq ($(CONFIG_WHC_CMD_PATH),y)
ccflags-$(CONFIG_WHC_CMD_PATH) += -DCONFIG_WHC_CMD_PATH
whc-ipc-objs += ./driver/whc_host_netlink.o
endif

else

KDIR ?= /lib/modules/$(shell uname -r)/build

# Regular build, build here.
M := $(CURDIR)
B := $(abspath $O/$M)

#fullmac
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
