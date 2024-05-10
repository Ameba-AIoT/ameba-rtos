
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/application/demoui
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
	-I$(BASEDIR)/component/ui/third_party/libpng/interfaces/include \
	-I$(BASEDIR)/component/ui/third_party/libjpeg-turbo/interfaces/include \
	-I$(BASEDIR)/component/ui/third_party/zlib/interfaces/include

MODULE_IFLAGS +=  -I$(DIR) \
	-I$(DIR)/app/ui \
	-I$(DIR)/utils/interfaces \
	-I$(DIR)/utils/src/resource \
	-I$(BASEDIR)/component/ui/LVGL/port/amebasmart \
	-I$(BASEDIR)/component/ui/LVGL/lv_drivers/interfaces \
        -I$(BASEDIR)/component/ui/LVGL/lvgl/demos/stress \

CPPFLAGS += -DLV_LVGL_H_INCLUDE_SIMPLE
GLOBAL_CFLAGS += -DLV_CONF_INCLUDE_SIMPLE

ifeq ($(CONFIG_IMAGE_SD_CARD),y)
	CPPFLAGS += -DCONFIG_IMAGE_SD_CARD
	GLOBAL_CFLAGS += -DCONFIG_IMAGE_SD_CARD
endif

vpath %.c $(DIR) $(shell find $(DIR) -type d)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#

CSRC += $(DIR)/test/demoui_example.c

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

