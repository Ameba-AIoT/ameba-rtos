
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/os/freertos/freertos_posix
SOURCE_DIR = $(DIR)/FreeRTOS-Plus-POSIX/test
UNITY_DIR = $(BASEDIR)/component/utils/unity

vpath %.c $(SOURCE_DIR) $(shell find $(SOURCE_DIR) -type d)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#

MODULE_IFLAGS += \
    -I$(DIR)/include \
    -I$(DIR)/FreeRTOS-Plus-POSIX/include \
    -I$(DIR)/FreeRTOS-Plus-POSIX/include/portable/realtek \
    -I$(FREERTOSDIR)/include \
    -I$(FREERTOSDIR)/portable/GCC/AmebaLite_KM4/non_secure \
    -I$(UNITY_DIR)/src \
    -I$(UNITY_DIR)/extras/bdd/src \
    -I$(UNITY_DIR)/extras/memory/src \
    -I$(UNITY_DIR)/extras/fixture/src

CSRC+= \
	$(SOURCE_DIR)/iot_test_posix_clock.c \
	$(SOURCE_DIR)/iot_test_posix_pthread.c \
	$(SOURCE_DIR)/iot_test_posix_stress.c \
	$(SOURCE_DIR)/iot_test_posix_unistd.c \
	$(SOURCE_DIR)/iot_test_posix_mqueue.c \
	$(SOURCE_DIR)/iot_test_posix_semaphore.c \
	$(SOURCE_DIR)/iot_test_posix_timer.c \
	$(SOURCE_DIR)/iot_test_posix_utils.c \
	$(SOURCE_DIR)/all_tests_runner.c

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
