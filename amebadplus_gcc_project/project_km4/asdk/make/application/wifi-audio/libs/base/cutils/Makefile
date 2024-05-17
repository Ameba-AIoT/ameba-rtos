include $(MAKE_INCLUDE_GEN)
include ../../config.mk

.PHONY: lib clean

#*****************************************************************************#
#                                  VARIABLES                                  #
#*****************************************************************************#
WFA_DIR = $(BASEDIR)/component/application/wifi-audio
DIR = $(WFA_DIR)/base/cutils

vpath %.c $(shell find $(DIR) -type d)
#*****************************************************************************#
#                               CFLAGS                                        #
#*****************************************************************************#
ifeq ($(CONFIG_WFA_FEC_USE_SRAM),y)
GLOBAL_CFLAGS += -DCONFIG_WFA_FEC_USE_SRAM
endif

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
MODULE_IFLAGS += \
    -I$(WFA_DIR)/base/log/include \
    -I$(WFA_DIR)/base/cutils/include

CSRC  = $(DIR)/wfa_mem.c
CSRC += $(DIR)/wfa_mutex.c
CSRC += $(DIR)/wfa_thread.c
CSRC += $(DIR)/wfa_time.c
CSRC += $(DIR)/wfa_timer.c
CSRC += $(DIR)/wfa_heap.c
CSRC += $(DIR)/wfa_imap.c
CSRC += $(DIR)/wfa_map.c
CSRC += $(DIR)/wfa_list.c
CSRC += $(DIR)/wfa_tlv.c
#CSRC += $(DIR)/wfa_ringbuffer.c

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                               Dependency	                                  #
#*****************************************************************************#
-include $(OBJS:.o=.d)

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
COPY_RAM_OBJS: CORE_TARGETS

lib: COPY_RAM_OBJS
	$(COPY) $(OBJS) ../../../xip/

#*****************************************************************************#
#              GENERATE OBJECT FILE
#*****************************************************************************#
CORE_TARGETS:	$(OBJS)

%.o:%.c
	$(Q)$(CC) $(GLOBAL_CFLAGS) $(MODULE_IFLAGS) $<  -o $@

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(call CLEAN_FUNC)
