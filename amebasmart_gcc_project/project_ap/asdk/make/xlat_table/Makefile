
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(TARGETDIR)/fwlib/ap_core/xlat_table
AP_DIR = $(TARGETDIR)/fwlib/ap_core

MODULE_IFLAGS += -I$(DIR)
GLOBAL_CFLAGS += -O3 
#-Wno-pointer-to-int-cast

vpath %.c $(DIR) $(AP_DIR)
vpath %.S $(DIR)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC = $(DIR)/xlat_tables_context.c \
       $(DIR)/xlat_tables_core.c \
       $(DIR)/xlat_tables_utils.c \
       $(DIR)/xlat_tables_arch.c

CSRC += $(AP_DIR)/ameba_xlat_table.c

ASM_SRC = $(DIR)/enable_mmu.S
ASM_SRC += $(DIR)/cache_helpers.S

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))
ASM_OBJS = $(notdir $(ASM_SRC:.S=.o))

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
COPY_RAM_OBJS: CORE_TARGETS
all: COPY_RAM_OBJS

#*****************************************************************************#
#              GENERATE OBJECT FILE 
#*****************************************************************************#
CORE_TARGETS:	$(OBJS) $(ASM_OBJS)

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS 
	$(call CLEAN_FUNC)

-include $(DEPS)	
