
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/os/freertos
DIR_SWLIB = $(BASEDIR)/component/soc/amebasmart/swlib
DIR_OSWRAPPER = $(BASEDIR)/component/os/freertos/os_wrapper

MODULE_IFLAGS += -I$(DIR_OSWRAPPER)/include

vpath freertos_heap5_config.c $(DIR)
vpath freertos_service.c $(DIR)
vpath %.c $(DIR_OSWRAPPER) $(DIR) $(shell find $(DIR) -type d) $(DIR_SWLIB)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC += $(DIR)/freertos_heap5_config.c \
       
CSRC += $(DIR_SWLIB)/gcc_wrap.c
CSRC += $(DIR_SWLIB)/locks.c

CSRC += $(DIR_OSWRAPPER)/os_wrapper_critical.c \
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
	$(REMOVE) *.o
	$(REMOVE) *.i
	$(REMOVE) *.s
	$(REMOVE) *.d

-include $(DEPS)
MODULE_IFLAGS += -I$(FREERTOSDIR)/include/
MODULE_IFLAGS += -I$(FREERTOSDIR)/portable/GCC/ARM_CA7/
