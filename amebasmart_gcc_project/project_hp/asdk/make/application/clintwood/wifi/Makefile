
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/application/clintwood/wifi
MODULE_IFLAGS += -I$(BASEDIR)/component/wifi/driver
MODULE_IFLAGS += -I$(BASEDIR)/component/wifi/driver/core
MODULE_IFLAGS += -I$(BASEDIR)/component/wifi/driver/core_rom
MODULE_IFLAGS += -I$(BASEDIR)/component/wifi/driver/include
MODULE_IFLAGS += -I$(BASEDIR)/component/wifi/driver/phl
MODULE_IFLAGS += -I$(BASEDIR)/component/wifi/driver/halmac
MODULE_IFLAGS += -I$(BASEDIR)/component/wifi/driver/halcommon
MODULE_IFLAGS += -I$(BASEDIR)/component/wifi/driver/halbtc
MODULE_IFLAGS += -I$(BASEDIR)/component/wifi/driver/phy_g6
MODULE_IFLAGS += -I$(BASEDIR)/component/wifi/driver/phy_g6/bb
MODULE_IFLAGS += -I$(BASEDIR)/component/wifi/driver/phy_g6/rf
MODULE_IFLAGS += -I$(BASEDIR)/component/wifi/driver/rtl8730e
MODULE_IFLAGS += -I$(BASEDIR)/component/wifi/driver/rtl8730e_rom
MODULE_IFLAGS += -I$(BASEDIR)/component/wifi/driver/core/option
MODULE_IFLAGS += -I$(BASEDIR)/component/wifi/driver/core/rom/mbedtls
MODULE_IFLAGS += -I$(BASEDIR)/component/wifi/wifi_fw/amebasmart/include
vpath %.c $(DIR)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC = $(DIR)/msft_wifi_twt.c
CSRC += $(DIR)/msft_wifi_twt_softap.c




#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
#all: CORE_TARGETS
all: CORE_TARGETS RENAME_LOADER_OBJS COPY_RAM_OBJS

#*****************************************************************************#
#              GENERATE OBJECT FILE
#*****************************************************************************#
CORE_TARGETS: $(OBJS)

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(call CLEAN_FUNC)

-include $(DEPS)
