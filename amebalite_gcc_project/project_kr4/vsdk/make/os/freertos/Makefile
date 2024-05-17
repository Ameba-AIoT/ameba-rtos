
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/os/freertos
DIR_OSDEP = $(BASEDIR)/component/os_dep
DIR_OSWRAPPER = $(BASEDIR)/component/os/freertos/os_wrapper

MODULE_IFLAGS += -I$(FREERTOSDIR)/include/
MODULE_IFLAGS += -I$(FREERTOSDIR)/portable/GCC/AmebaLite_KR4/
MODULE_IFLAGS += -I$(DIR_OSWRAPPER)/include

vpath %.c $(DIR_OSWRAPPER) $(DIR) $(DIR_OSDEP)
#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC = $(DIR)/freertos_heap5_config.c \

CSRC+= $(DIR_OSWRAPPER)/os_wrapper_critical.c \
       $(DIR_OSWRAPPER)/os_wrapper_memory.c \
       $(DIR_OSWRAPPER)/os_wrapper_mutex.c \
       $(DIR_OSWRAPPER)/os_wrapper_queue.c \
       $(DIR_OSWRAPPER)/os_wrapper_semaphore.c \
       $(DIR_OSWRAPPER)/os_wrapper_task.c \
       $(DIR_OSWRAPPER)/os_wrapper_time.c \
       $(DIR_OSWRAPPER)/os_wrapper_timer.c \
       $(DIR_OSWRAPPER)/os_wrapper_static_functions.c

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
CORE_TARGETS: $(OBJS)

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS 
	$(call CLEAN_FUNC)
	
-include $(DEPS)



