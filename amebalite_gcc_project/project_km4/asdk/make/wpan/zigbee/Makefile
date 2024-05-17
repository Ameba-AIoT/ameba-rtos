include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                                  #
#*****************************************************************************#
ZB_DIR				= $(BASEDIR)/component/wpan/zigbee
ZBOSS_LIB_DIR		= $(BASEDIR)/component/wpan/zigbee/lib
TIMER				= $(BASEDIR)/component/wpan/platform/timer
FLASH				= $(BASEDIR)/component/wpan/platform/flash
SW_RADIO			= $(BASEDIR)/component/wpan/platform/sw_radio
ZB_RTK_SUPPORT_DIR	= $(BASEDIR)/component/wpan/zigbee/rtk_zb_support

ZB_API_DIR		= $(BASEDIR)/component/wpan/zigbee/api
ZB_STACK_DIR	= $(BASEDIR)/component/wpan/zigbee/zboss_stable

MAKE_LIB		= $(AR) rcv $@ $^

%.gppb.o:%.c
	$(ECHO) "  CC      $<"
	$(Q)$(CC) $(GLOBAL_CFLAGS) $(MODULE_IFLAGS) $<  -o $@

%.ed.o:%.c
	$(ECHO) "  CC      $<"
	$(Q)$(CC) $(GLOBAL_CFLAGS) $(MODULE_IFLAGS) $<  -o $@

vpath %.c $(ZB_API_DIR)/ \
		  $(ZB_RTK_SUPPORT_DIR)/

MODULE_IFLAGS += -I$(ZB_DIR)/include/
MODULE_IFLAGS += -I$(ZB_RTK_SUPPORT_DIR)/config/
MODULE_IFLAGS += -I$(ZB_RTK_SUPPORT_DIR)/include/

MODULE_IFLAGS += -I$(SW_RADIO)/phy/common/radio_algorithm/include/
MODULE_IFLAGS += -I$(SW_RADIO)/phy/common/radio_api/
MODULE_IFLAGS += -I$(SW_RADIO)/mac/
MODULE_IFLAGS += -I$(SW_RADIO)/mac/config
MODULE_IFLAGS += -I$(SW_RADIO)/mac/mac_features/
MODULE_IFLAGS += -I$(SW_RADIO)/mac/mac_features/ack_generator/
MODULE_IFLAGS += -I$(SW_RADIO)/mac/rsch/

MODULE_IFLAGS += -I$(TIMER)/
MODULE_IFLAGS += -I$(TIMER)/config/

MODULE_IFLAGS += -I$(FLASH)/
MODULE_IFLAGS += -I$(FLASH)/config/


# zigbee stack
include Makefile_stack_base

# osif	
CSRC += \
		$(ZB_RTK_SUPPORT_DIR)/zb_osif_entry.c \
		$(ZB_RTK_SUPPORT_DIR)/zb_osif_common.c \
		$(ZB_RTK_SUPPORT_DIR)/zb_osif_logger.c \
		$(ZB_RTK_SUPPORT_DIR)/zb_osif_nvram.c \
		$(ZB_RTK_SUPPORT_DIR)/zb_osif_transceiver.c \
		$(ZB_RTK_SUPPORT_DIR)/zb_osif_transceiver_mem.c \
		$(ZB_RTK_SUPPORT_DIR)/zb_osif_timer.c \

# api
CSRC += \
		$(ZB_API_DIR)/common/zigbee_helpers.c \
		$(ZB_API_DIR)/common/zb_error_to_string.c \
		
GLOBAL_CFLAGS += \
		-DZB_ERROR_PRINT_TO_LOG \
		-DCONFIG_ZIGBEE_ERROR_TO_STRING_ENABLED	\
		-DLIBZBOSS_CONFIG_FILE=\"libzboss_config.h\"
		# -DCONFIG_ZB_TRACE_LOG_LEVEL=2 \
		# -DCONFIG_ZB_TRACE_MASK=0xFFFF \
		
ifeq ($(CONFIG_802154_ZIGBEE_ROLE_ZED), y)
GLOBAL_CFLAGS += \
		-DZB_ED_ROLE
endif


OBJS = $(notdir $(CSRC:.c=.o))

# LIBS
ifeq ($(CONFIG_802154_ZIGBEE_MAC_TEST_EN), y)
GLOBAL_CFLAGS += -DCONFIG_ZB_TRACE_LOG_LEVEL=4 -DCONFIG_ZB_TRACE_MASK=0x8000
include Makefile_mac_cert
endif

ifeq ($(CONFIG_802154_ZIGBEE_ZCP_TEST_EN), y)
GLOBAL_CFLAGS += -DCONFIG_ZB_TRACE_LOG_LEVEL=4 -DCONFIG_ZB_TRACE_MASK=0x0840
include Makefile_r22_cert
endif

ifeq ($(CONFIG_802154_ZIGBEE_REGRESSION_TEST_EN), y)
GLOBAL_CFLAGS += -DCONFIG_ZB_TRACE_LOG_LEVEL=4 -DCONFIG_ZB_TRACE_MASK=0x0954
endif

ifeq ($(CONFIG_802154_ZIGBEE_APPLICATION_EN), y)
#CONFIG_ZB_TRACE_LOG_LEVEL, CONFIG_ZB_TRACE_MASK define refer to libzboss_amebalite_config(.ed).h
include Makefile_application
endif

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

#LIBBLUETOOTH_OBJS_PATH = $(shell pwd)/objs
# Define the Rules to build the core targets
all: CORE_TARGETS COPY_RAM_OBJS

ifeq ($(CONFIG_802154_ZIGBEE_ROLE_ZC_ZR), y)
lib: $(ZBOSS_LIB)
	$(MOVE) -f $(ZBOSS_LIB) $(ZBOSS_LIB_DIR)/
endif

ifeq ($(CONFIG_802154_ZIGBEE_ROLE_ZED), y)
lib: $(ZBOSS_LIB_ED)
	$(MOVE) -f $(ZBOSS_LIB_ED) $(ZBOSS_LIB_DIR)/
endif

# regression test

#*****************************************************************************#
#              GENERATE OBJECT FILE
#*****************************************************************************#
CORE_TARGETS: $(OBJS)

$(ZBOSS_LIB):$(LIBS)
	rm -rf ar-tmp
	mkdir -p ar-tmp
	cd ar-tmp && for i in $(LIBS); do $(AR) x ../$$i; done && cd ..
	$(AR) rcv $@ ar-tmp/*o


$(ZBOSS_LIB_ED):$(LIBS_ED)
	rm -rf ar-tmp
	mkdir -p ar-tmp
	cd ar-tmp && for i in $(LIBS_ED); do $(AR) x ../$$i; done && cd ..
	$(AR) rcv $@ ar-tmp/*o


#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(REMOVE) *.o
	$(REMOVE) *.i
	$(REMOVE) *.s
	$(REMOVE) *.d
	$(REMOVE) *.su
	$(REMOVE) ar-tmp

-include $(DEPS)