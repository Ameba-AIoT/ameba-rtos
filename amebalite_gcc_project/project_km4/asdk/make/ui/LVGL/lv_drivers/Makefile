
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/ui/LVGL/lv_drivers/amebalite
DIRLV = $(BASEDIR)/component/ui/LVGL/lvgl

MODULE_IFLAGS += \
	-I$(BASEDIR)/component/ui/LVGL/lv_drivers/interfaces \
	-I$(BASEDIR)/component/ui/drivers/st7789v/include \
	-I$(BASEDIR)/component/ui/LVGL/port/amebalite\
	-I$(DIRLV)/src \
	-I$(DIRLV)/src/core \
	-I$(DIRLV)/src/draw \
	-I$(DIRLV)/src/extra \
	-I$(DIRLV)/src/font \
	-I$(DIRLV)/src/hal \
	-I$(DIRLV)/src/misc \
	-I$(DIRLV)/src/widgets \

vpath %.c $(DIR) $(shell find $(DIR) -type d)
#*****************************************************************************#
#                               CFLAGS                                        #
#*****************************************************************************#
GLOBAL_CFLAGS += -D__RTOS__ -Wno-incompatible-pointer-types

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC := $(DIR)/src/display.c \
	$(DIR)/src/lv_hal.c

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#
all: CLEAN_OBJS CORE_TARGETS COPY_RAM_OBJS

#*****************************************************************************#
#              GENERATE OBJECT FILE
#*****************************************************************************#
CORE_TARGETS: $(OBJS)

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(REMOVE) *.o
	$(REMOVE) *.i
	$(REMOVE) *.s
	$(REMOVE) *.d

-include $(DEPS)

