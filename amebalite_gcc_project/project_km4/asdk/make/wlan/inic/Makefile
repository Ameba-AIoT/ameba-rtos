
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

IFLAGS               += -I$(BASEDIR)/component/wifi/common
IFLAGS               += -I$(BASEDIR)/component/wifi/driver
IFLAGS               += -I$(BASEDIR)/component/wifi/driver/halmac
IFLAGS               += -I$(BASEDIR)/component/wifi/driver/halcommon
IFLAGS               += -I$(BASEDIR)/component/wifi/driver/halbtc
IFLAGS               += -I$(BASEDIR)/component/wifi/driver/phl

ifeq ($(CONFIG_AMEBALITE), y)
IFLAGS               += -I$(BASEDIR)/component/wifi/driver/phy_g6/bb
IFLAGS               += -I$(BASEDIR)/component/wifi/driver/phy_g6/rf
IFLAGS               += -I$(BASEDIR)/component/wifi/driver/rtl8720e
IFLAGS               += -I$(BASEDIR)/component/wifi/driver/rtl8720e_rom
IFLAGS               += -I$(BASEDIR)/component/wifi/wifi_fw/amebalite/include
endif

ifeq ($(CONFIG_AMEBADPLUS), y)
IFLAGS               += -I$(BASEDIR)/component/wifi/driver/phydm
IFLAGS               += -I$(BASEDIR)/component/wifi/driver/rtl8721da
IFLAGS               += -I$(BASEDIR)/component/wifi/driver/rtl8721da_rom
IFLAGS               += -I$(BASEDIR)/component/wifi/wifi_fw/amebadplus/include
endif

ifeq ($(CONFIG_AMEBASMART), y)
IFLAGS               += -I$(BASEDIR)/component/wifi/driver/phy_g6
IFLAGS               += -I$(BASEDIR)/component/wifi/driver/phy_g6/bb
IFLAGS               += -I$(BASEDIR)/component/wifi/driver/phy_g6/rf
IFLAGS               += -I$(BASEDIR)/component/wifi/driver/rtl8730e
IFLAGS               += -I$(BASEDIR)/component/wifi/driver/rtl8730e_rom
IFLAGS               += -I$(BASEDIR)/component/wifi/wifi_fw/amebasmart/include
endif

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
INICDRV = $(BASEDIR)/component/wifi/inic/ipc
OSSER = $(BASEDIR)/component/os/os_dep
OSCER = $(BASEDIR)/component/wifi/driver/core
COMDIR = $(BASEDIR)/component/wifi/common

vpath %.c $(INICDRV) $(OSSER) $(OSCER) $(COMDIR)
#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#

# IPC HOST
CSRC  = $(INICDRV)/inic_ipc_host.c \
	$(INICDRV)/inic_ipc_host_trx.c \
	$(INICDRV)/inic_ipc_host_api.c \
	$(INICDRV)/inic_ipc_host_api_basic.c \
	$(INICDRV)/inic_ipc_host_api_ext.c \
	$(INICDRV)/inic_ipc_msg_queue.c

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
# only AP core will make lib/lib_mp
lib_ipc: CORE_TARGETS
	$(AR) rvs lib_inic.a $(OBJS)
	$(FROMELF) -g -D lib_inic.a
ifeq ($(CONFIG_MP_INCLUDED), y)
ifeq ($(CONFIG_MP_SHRINK), y)
	$(MOVE) -f lib_inic.a $(ROOTDIR)/lib/application/lib_wifi_inic_ap_mp_shrink.a
else
	$(MOVE) -f lib_inic.a $(ROOTDIR)/lib/application/lib_wifi_inic_ap_mp.a
endif
else
	$(MOVE) -f lib_inic.a $(ROOTDIR)/lib/application/lib_wifi_inic_ap.a
endif

#*****************************************************************************#
#              GENERATE OBJECT FILE
#*****************************************************************************#
CORE_TARGETS:	$(OBJS) $(ASM_OBJS)


#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(call CLEAN_FUNC)

-include $(DEPS)
