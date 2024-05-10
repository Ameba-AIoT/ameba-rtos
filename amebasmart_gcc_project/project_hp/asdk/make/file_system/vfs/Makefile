
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/file_system/vfs
MODULE_IFLAGS += -I$(BASEDIR)/component/sdio/sd_host/inc

vpath %.c $(DIR)
#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC += $(DIR)/vfs_littlefs.c
CSRC += $(DIR)/vfs_wrap.c
CSRC += $(DIR)/vfs.c
ifeq ($(CONFIG_VFS_FATFS_INCLUDED),y)
CSRC += $(DIR)/vfs_fatfs.c
CSRC += $(DIR)/vfs_fatfs_sdcard_api.c
CSRC += $(DIR)/vfs_fatfs_sdcard.c
CSRC += $(DIR)/vfs_fatfs_flash.c
CSRC += $(DIR)/vfs_fatfs_flash_api.c
endif
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
