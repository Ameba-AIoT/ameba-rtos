
include $(MAKE_INCLUDE_GEN)
IFLAGS               += -I$(BASEDIR)/component/wifi/common
IFLAGS               += -I$(BASEDIR)/component/wifi/driver
IFLAGS               += -I$(BASEDIR)/component/wifi/driver/halcommon
IFLAGS               += -I$(BASEDIR)/component/wifi/driver/halbtc
IFLAGS               += -I$(BASEDIR)/component/wifi/driver/phl
IFLAGS               += -I$(BASEDIR)/component/wifi/driver/core
IFLAGS               += -I$(BASEDIR)/component/wifi/driver/intf
IFLAGS               += -I$(BASEDIR)/component/wifi/common/secure

ifeq ($(CONFIG_MP_INCLUDED), y)
GLOBAL_CFLAGS += -Wno-unused-function
endif

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
ifeq ($(CONFIG_INIC_INTF_IPC), y)
IFLAGS               += -I$(BASEDIR)/component/wifi/inic/ipc
else
IFLAGS               += -I$(BASEDIR)/component/wifi/inic/sdio
endif
IFLAGS               += -I$(FREERTOSDIR)/include/
IFLAGS               += -I$(FREERTOSDIR)/portable/GCC/AmebaDplus_KM0/
endif

ifeq ($(CONFIG_AMEBASMART), y)
IFLAGS               += -I$(TARGETDIR)/fwlib/ap_core
IFLAGS               += -I$(BASEDIR)/component/wifi/driver/phy_g6
IFLAGS               += -I$(BASEDIR)/component/wifi/driver/phy_g6/bb
IFLAGS               += -I$(BASEDIR)/component/wifi/driver/phy_g6/rf
IFLAGS               += -I$(BASEDIR)/component/wifi/driver/rtl8730e
IFLAGS               += -I$(BASEDIR)/component/wifi/driver/rtl8730e_rom
IFLAGS               += -I$(BASEDIR)/component/wifi/wifi_fw/amebasmart/include
IFLAGS               += -I$(FREERTOSDIR)/include/
IFLAGS               += -I$(FREERTOSDIR)/portable/GCC/AmebaSmart_HP/non_secure
IFLAGS               += -I$(FREERTOSDIR)/portable/GCC/AmebaSmart_HP/secure
endif

.PHONY: lib lib_mp clean make_objs_drv make_objs_phy make_objs_btc
#*****************************************************************************#
#                                   VARIABLES                                 #
#*****************************************************************************#
DIR = $(BASEDIR)/component/wifi/driver

ifeq ($(CONFIG_MP_INCLUDED), y)
LIB_NAME_MP_SUFFIX = _mp
endif
# OBJS_CONFIG: code run flash or sram based on config
# OBJS: Flash code
# OBJS_SRAM: SRAM code

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#

include Makefile.drv
include Makefile.phy
include Makefile.btc

CSRC += $(CSRC_DRV) $(CSRC_PHY) $(CSRC_BTC)

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                                 Dependency                                  #
#*****************************************************************************#
-include $(OBJS:.o=.d)

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
COPY_RAM_OBJS: CORE_TARGETS

lib: make_objs_drv make_objs_phy make_objs_btc COPY_RAM_OBJS
	$(AR) rvs lib_wlan.a $(OBJS)
	$(FROMELF) -g -D lib_wlan.a
ifeq ($(CONFIG_AS_INIC_NP),y)
ifeq ($(CONFIG_INIC_INTF_SDIO), y)
	$(MOVE) -f lib_wlan.a $(ROOTDIR)/lib/application/lib_wifi_inic_sdio_np$(LIB_NAME_MP_SUFFIX).a
else
	$(MOVE) -f lib_wlan.a $(ROOTDIR)/lib/application/lib_wifi_inic_np$(LIB_NAME_MP_SUFFIX).a
endif
else ifeq ($(CONFIG_SINGLE_CORE_WIFI),y)
ifeq ($(CONFIG_SDIO_BRIDGE),y)
	$(MOVE) -f lib_wlan.a $(ROOTDIR)/lib/application/lib_wifi_sdio_bridge$(LIB_NAME_MP_SUFFIX).a
else
	$(MOVE) -f lib_wlan.a $(ROOTDIR)/lib/application/lib_wifi_single_core$(LIB_NAME_MP_SUFFIX).a
endif
endif
	$(REMOVE) $(TARGET_LIB)_git_version.*

#*****************************************************************************#
#              GENERATE OBJECT FILE                                           #
#*****************************************************************************#
CORE_TARGETS:	$(OBJS)

#*****************************************************************************#
#              GENERATE GIT VERSION FILE                                      #
#*****************************************************************************#
$(TARGET_LIB)_git_version.c:
	$(call GEN_GIT_VER, $(TARGET_LIB))

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(call CLEAN_FUNC)
