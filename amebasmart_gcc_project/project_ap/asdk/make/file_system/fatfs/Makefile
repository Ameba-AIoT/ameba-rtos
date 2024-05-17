
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/file_system/fatfs
MODULE_IFLAGS += -I$(BASEDIR)/component/usb/host/msc/inc
MODULE_IFLAGS += -I$(BASEDIR)/component/sdio/sd_host/inc

vpath %.c $(shell find $(DIR)/r0.14b/src/ -type d) $(DIR)/disk_if/src $(DIR)/fatfs_ext/src $(DIR)
#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC = $(DIR)/disk_if/src/usbdisk.c \
	$(DIR)/r0.14b/src/ff.c \
	$(DIR)/r0.14b/src/diskio.c \
	$(DIR)/r0.14b/src/ffsystem.c \
	$(DIR)/r0.14b/src/ffunicode.c  

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
COPY_RAM_OBJS: CORE_TARGETS
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
