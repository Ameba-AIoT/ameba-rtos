
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/os/freertos/freertos_v10.4.3
DIR_PORT = $(DIR)/Source/portable/GCC/AmebaLite_KR4/
MODULE_IFLAGS += -I$(FREERTOSDIR)/include/
MODULE_IFLAGS += -I$(FREERTOSDIR)/portable/GCC/AmebaLite_KR4/

ifeq ($(CONFIG_WIFI_EN),y)
DIR_OSDEP = $(BASEDIR)/component/os_dep
endif

vpath portASM.S $(DIR_PORT)
vpath %.c $(DIR_PORT)
vpath %.c $(DIR_OSDEP) $(shell find $(DIR_OSDEP) -type d)
vpath %.c $(DIR)/Source
vpath %.c $(DIR)/Source/portable/MemMang/

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC = $(DIR)/Source/tasks.c \
       $(DIR)/Source/list.c \
       $(DIR)/Source/croutine.c \
       $(DIR)/Source/queue.c \
       $(DIR)/Source/timers.c \
       $(DIR)/Source/event_groups.c \
       $(DIR)/Source/stream_buffer.c \


CSRC += $(DIR_PORT)/port.c \
       $(DIR_PORT)/rtk-port.c

#CSRC+= $(DIR)/Source/portable/MemMang/heap_2.c
CSRC+= $(DIR)/Source/portable/MemMang/heap_5.c


#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))
OBJS += portASM.o

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
#all: CORE_TARGETS COPY_RAM_OBJS
all: CORE_TARGETS COPY_RAM_OBJS
	
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
