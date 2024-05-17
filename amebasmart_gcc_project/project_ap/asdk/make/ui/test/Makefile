
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/ui/LVGL/lv_drivers/amebasmart/test
DIRLV = $(BASEDIR)/component/ui/LVGL/lvgl

MODULE_IFLAGS +=  -I$(DIRLV) \
	-I$(DIRLV)/src \
	-I$(DIRLV)/src/core \
	-I$(DIRLV)/src/draw \
	-I$(DIRLV)/src/extra \
	-I$(DIRLV)/src/font \
	-I$(DIRLV)/src/hal \
	-I$(DIRLV)/src/misc \
	-I$(DIRLV)/src/widgets \

MODULE_IFLAGS +=  -I$(DIR) \
	-I$(BASEDIR)/component/ui/LVGL/port/amebasmart \
	-I$(BASEDIR)/component/ui/LVGL/lv_drivers/interfaces

CPPFLAGS += -DLV_LVGL_H_INCLUDE_SIMPLE
GLOBAL_CFLAGS += -DLV_CONF_INCLUDE_SIMPLE

vpath %.c $(DIR) $(shell find $(DIR) -type d)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#

CSRC += $(DIR)/touch_example.c

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))

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
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(REMOVE) *.o
	$(REMOVE) *.ii
	$(REMOVE) *.i
	$(REMOVE) *.s
	$(REMOVE) *.d

-include $(DEPS)

