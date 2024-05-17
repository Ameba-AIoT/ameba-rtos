
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(TARGETDIR)/bootloader
CFGDIR = $(TARGETDIR)/usrcfg
DIR_COM = $(TARGETDIR)/fwlib/ram_common
DIR_RAM_LP = $(TARGETDIR)/fwlib/ram_km0
MODULE_IFLAGS = -I$(DIR)/../include/

vpath %.c $(DIR) $(CFGDIR) $(DIR_RAM_LP) $(DIR_COM)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC = $(DIR)/bootloader_km0.c

CSRC += $(CFGDIR)/ameba_bootcfg.c

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
#all: CORE_TARGETS
all: CORE_TARGETS COPY_RAM_OBJS

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
