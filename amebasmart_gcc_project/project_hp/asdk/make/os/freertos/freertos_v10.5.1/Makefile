
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/os/freertos/freertos_v10.5.1
DIR_PORT = $(DIR)/Source/portable/GCC/AmebaSmart_HP/non_secure

MODULE_IFLAGS += -I$(FREERTOSDIR)/include/
MODULE_IFLAGS += -I$(FREERTOSDIR)/portable/GCC/AmebaSmart_HP/non_secure
MODULE_IFLAGS += -I$(FREERTOSDIR)/portable/GCC/AmebaSmart_HP/secure

vpath port.c $(DIR_PORT)
vpath portasm.c $(DIR_PORT)
vpath %.c $(DIR) $(shell find $(DIR) -type d)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC = $(DIR)/Source/tasks.c \
       $(DIR)/Source/list.c \
       $(DIR)/Source/croutine.c \
       $(DIR)/Source/queue.c \
       $(DIR)/Source/timers.c \
       $(DIR)/Source/event_groups.c \
       $(DIR)/Source/stream_buffer.c

CSRC+= $(DIR_PORT)/port.c

CSRC+= $(DIR)/Source/portable/MemMang/heap_5.c

ifeq ($(CONFIG_TRUSTZONE_EN),y)
CSRC+= $(DIR_PORT)/portasm.c
else
CSRC+= $(DIR_PORT)/portasm_ntz.c
endif

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
NETWORK_RENAME_CODE2SRAM: CORE_TARGETS
COPY_RAM_OBJS: NETWORK_RENAME_CODE2SRAM
all: COPY_RAM_OBJS

#*****************************************************************************#
#              GENERATE OBJECT FILE 
#*****************************************************************************#
CORE_TARGETS:	$(OBJS)

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(call CLEAN_FUNC)

-include $(DEPS)
