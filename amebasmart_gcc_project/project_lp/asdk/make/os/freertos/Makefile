
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/os/freertos/freertos_v10.5.1
DIR_PORT = $(DIR)/Source/portable/GCC/AmebaSmart_LP
DIR_OSDEP = $(BASEDIR)/component/os_dep
DIR_OSWRAPPER = $(BASEDIR)/component/os/freertos/os_wrapper

MODULE_IFLAGS += -I$(FREERTOSDIR)/include/
MODULE_IFLAGS += -I$(FREERTOSDIR)/portable/GCC/AmebaSmart_LP/

vpath freertos_heap5_config.c $(DIR)/..
vpath freertos_service.c $(DIR)/..

vpath %.c  $(DIR_OSWRAPPER) $(DIR) $(DIR_OSDEP) $(DIR_PORT) $(shell find $(DIR) -type d)

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
       $(DIR)/../freertos_heap5_config.c

CSRC+= $(DIR_PORT)/port.c
CSRC+= $(DIR_PORT)/portasm.c

CSRC+= $(DIR)/Source/portable/MemMang/heap_5.c

MODULE_IFLAGS += -I$(DIR_OSWRAPPER)/include
CSRC+= $(DIR_OSWRAPPER)/os_wrapper_critical.c \
       $(DIR_OSWRAPPER)/os_wrapper_memory.c \
       $(DIR_OSWRAPPER)/os_wrapper_mutex.c \
       $(DIR_OSWRAPPER)/os_wrapper_queue.c \
       $(DIR_OSWRAPPER)/os_wrapper_semaphore.c \
       $(DIR_OSWRAPPER)/os_wrapper_task.c \
       $(DIR_OSWRAPPER)/os_wrapper_time.c \
       $(DIR_OSWRAPPER)/os_wrapper_timer.c\
       $(DIR_OSWRAPPER)/os_wrapper_static_functions.c

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
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
