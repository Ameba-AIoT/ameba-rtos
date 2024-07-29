# SPDX-License-Identifier: GPL-2.0-only
ccflags-$(CONFIG_NAN) += -DCONFIG_NAN
ccflags-y += -DCONFIG_FULLMAC -DCONFIG_FULLMAC_HCI_IPC
ccflags-y += -I$(srctree)/../../firmware/source/component/wifi/driver/include
ccflags-y += -I$(srctree)/../../firmware/source/component/wifi/api
ccflags-y += -I$(srctree)/../../firmware/source/component/wifi/inic/ipc
ccflags-y += -I$(srctree)/../../firmware/source/component/wifi/inic
ccflags-y += -I$(srctree)/$(src)/..

llhw-ipc-y := rtw_llhw_hci.o \
		rtw_llhw_ops.o \
		rtw_llhw_event.o \
		rtw_llhw_recv.o \
		rtw_llhw_xmit.o \
		rtw_llhw_msg.o \
		rtw_llhw_memory.o \
		ameba_wificfg.o
 
obj-y += llhw-ipc.o

