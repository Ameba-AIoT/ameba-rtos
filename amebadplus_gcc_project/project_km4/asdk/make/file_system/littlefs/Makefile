
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/file_system/littlefs
MODULE_IFLAGS += -I$(BASEDIR)/component/file_system/littlefs
MODULE_IFLAGS += -I$(BASEDIR)/component/file_system/littlefs/r2.50
MODULE_IFLAGS += -I$(BASEDIR)/component/file_system/ftl_common

vpath %.c $(DIR) $(DIR)/r2.50
#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC += $(DIR)/littlefs_adapter.c
#CSRC += $(DIR)/lfs_nor_flash_api.c
CSRC += $(DIR)/r2.50/lfs.c
CSRC += $(DIR)/r2.50/lfs_util.c


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
	$(call CLEAN_FUNC)

-include $(DEPS)	
