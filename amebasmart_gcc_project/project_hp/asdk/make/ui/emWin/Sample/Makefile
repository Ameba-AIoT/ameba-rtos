
include $(MAKE_INCLUDE_GEN)


.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/ui/emWin/emwin/Sample/rtl8721d

MODULE_IFLAGS := -I$(DIR)./
MODULE_IFLAGS += -I$(BASEDIR)/component/ui/emWin/emwin/Sample/rtl8721d/Config
MODULE_IFLAGS += -I$(BASEDIR)/component/ui/emWin/emwin/Sample/rtl8721d/hal
	
vpath %.c $(DIR)/Config $(DIR)/hal

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC = $(DIR)/Config/GUIConf.c \
	$(DIR)/hal/hal_touch.c \
	$(DIR)/hal/gt911.c \
	$(DIR)/hal/hal_rgb_lcd.c \
	$(DIR)/Config/LCDConf_RGB_6bit_eval.c \
#	$(DIR)/hal/ft6236.c \
#	$(DIR)/hal/hal_mcu_lcd.c \
#	$(DIR)/Config/LCDConf_MCU_8bit_eval.c \



#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                               Include Dependency                            #
#*****************************************************************************#
-include $(OBJS:.o=.d)

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#
COPY_RAM_OBJS: CORE_TARGETS
all: COPY_RAM_OBJS

#*****************************************************************************#
#              GENERATE OBJECT FILE 
#*****************************************************************************#
CORE_TARGETS: $(OBJS)

#*****************************************************************************#
#              CLEAN GENERATED FILES                                          #
#*****************************************************************************#
clean: CLEAN_OBJS 
	$(call CLEAN_FUNC)

-include $(DEPS)
