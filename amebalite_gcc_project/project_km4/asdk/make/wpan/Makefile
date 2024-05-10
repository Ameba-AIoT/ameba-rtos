
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
TIMER = $(BASEDIR)/component/wpan/platform/timer
FLASH = $(BASEDIR)/component/wpan/platform/flash
PLATFORM_CONFIG = $(BASEDIR)/component/wpan/platform/config

vpath %.c $(TIMER) $(shell find $(TIMER) -type d)
vpath %.c $(FLASH) $(shell find $(FLASH) -type d)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
ifneq ($(CONFIG_802154_THREAD_RADIO_RCP_EN),y)	
include Makefile_mac
endif
#802154 timer
CSRC += $(TIMER)/rtw_802154_timer.c \
		$(TIMER)/rtw_802154_timer_sched.c
		
CSRC += $(FLASH)/rtw_802154_flash.c

MODULE_IFLAGS += -I$(TIMER)/
MODULE_IFLAGS += -I$(FLASH)/
MODULE_IFLAGS += -I$(PLATFORM_CONFIG)/
MODULE_IFLAGS += -I$(BASEDIR)/component/soc/amebalite/fwlib/include
MODULE_IFLAGS += -I$(BASEDIR)/component/os/os_dep/

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
all: CORE_TARGETS COPY_RAM_OBJS
ifeq ($(CONFIG_802154_THREAD_EN),y)	
	make -C openthread lib
endif
ifeq ($(CONFIG_802154_ZIGBEE_EN),y)	
	make -C zigbee all
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

ifeq ($(CONFIG_802154_THREAD_EN),y)	
	make -C openthread clean
endif
ifeq ($(CONFIG_802154_ZIGBEE_EN),y)	
	make -C zigbee clean
endif	
-include $(DEPS)
