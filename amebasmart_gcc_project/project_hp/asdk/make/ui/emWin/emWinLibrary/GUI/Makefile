
include $(MAKE_INCLUDE_GEN)


.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/ui/emWin/emwin/emWinLibrary/GUI
TARGET_LIB = lib_emwin

MODULE_IFLAGS := -I$(DIR)./
MODULE_IFLAGS += -I$(BASEDIR)/component/ui/emWin/emwin/Sample/rtl8721d/Config
MODULE_IFLAGS += -I$(BASEDIR)/component/ui/emWin/emwin/Sample/rtl8721d/hal
	
vpath %.c $(shell find $(DIR) -type d)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC_LIB += $(wildcard $(DIR)/AntiAlias/*.c) \
	$(wildcard $(DIR)/ConvertColor/*.c) \
	$(wildcard $(DIR)/ConvertMono/*.c) \
	$(wildcard $(DIR)/Core/*.c) \
	$(wildcard $(DIR)/DisplayDriver/*.c) \
	$(wildcard $(DIR)/Font/*.c) \
	$(wildcard $(DIR)/MemDev/*.c) \
	$(wildcard $(DIR)/TouchDriver/*.c) \
	$(wildcard $(DIR)/Widget/*.c) \
	$(wildcard $(DIR)/WM/*.c) \

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS_LIB = $(notdir $(CSRC_LIB:.c=.o))

#*****************************************************************************#
#                               Include Dependency                            #
#*****************************************************************************#
-include $(OBJS:.o=.d)

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#
lib: CORE_TARGETS_LIB
	$(AR) rvs lib_user.a $(OBJS) $(OBJS_SRAM)
	$(FROMELF) -g -D lib_user.a
	$(MOVE) -f lib_user.a $(ROOTDIR)/lib/application

#*****************************************************************************#
#              GENERATE OBJECT FILE 
#*****************************************************************************#
CORE_TARGETS_LIB: $(OBJS_LIB)

#*****************************************************************************#
#              CLEAN GENERATED FILES                                          #
#*****************************************************************************#
clean: CLEAN_OBJS 
	$(call CLEAN_FUNC)

-include $(DEPS)
