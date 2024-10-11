
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/os/freertos/freertos_posix
SOURCE_DIR = $(DIR)/FreeRTOS-Plus-POSIX/source

vpath %.c $(SOURCE_DIR) $(shell find $(SOURCE_DIR) -type d)

#*****************************************************************************#
#                               CFLAGS                                        #
#*****************************************************************************#
# avoid compile warnings
GLOBAL_CFLAGS += -Wno-cast-function-type

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#

IFLAGS += \
	-I$(DIR)/include \
	-I$(DIR)/FreeRTOS-Plus-POSIX/include \
	-I$(DIR)/FreeRTOS-Plus-POSIX/include/portable/realtek

IFLAGS += \
	-I$(FREERTOSDIR)/include \
	-I$(FREERTOSDIR)/portable/GCC/AmebaLite_KM4/non_secure \
	-I$(FREERTOSDIR)/portable/GCC/AmebaLite_KM4/secure

CSRC+= \
	$(SOURCE_DIR)/FreeRTOS_POSIX_clock.c \
	$(SOURCE_DIR)/FreeRTOS_POSIX_pthread_cond.c \
	$(SOURCE_DIR)/FreeRTOS_POSIX_timer.c \
	$(SOURCE_DIR)/FreeRTOS_POSIX_mqueue.c \
	$(SOURCE_DIR)/FreeRTOS_POSIX_pthread_mutex.c \
	$(SOURCE_DIR)/FreeRTOS_POSIX_unistd.c \
	$(SOURCE_DIR)/FreeRTOS_POSIX_pthread_barrier.c \
	$(SOURCE_DIR)/FreeRTOS_POSIX_sched.c \
	$(SOURCE_DIR)/FreeRTOS_POSIX_utils.c \
	$(SOURCE_DIR)/FreeRTOS_POSIX_pthread.c \
	$(SOURCE_DIR)/FreeRTOS_POSIX_semaphore.c

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
