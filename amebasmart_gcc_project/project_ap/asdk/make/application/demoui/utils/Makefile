
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean
TARGET_LIB = lib_ui_utils

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
	-I$(BASEDIR)/component/ui/LVGL/lv_drivers/interfaces

CPPFLAGS += -DLV_LVGL_H_INCLUDE_SIMPLE
GLOBAL_CFLAGS += -DLV_CONF_INCLUDE_SIMPLE

ifeq ($(CONFIG_IMAGE_LITTLEFS),y)
	CPPFLAGS += -DCONFIG_IMAGE_LITTLEFS
	GLOBAL_CFLAGS += -DCONFIG_IMAGE_LITTLEFS
endif
ifeq ($(CONFIG_IMAGE_SD_CARD),y)
	CPPFLAGS += -DCONFIG_IMAGE_SD_CARD
	GLOBAL_CFLAGS += -DCONFIG_IMAGE_SD_CARD
endif
ifeq ($(CONFIG_IMAGE_RAW_DATA),y)
	CPPFLAGS += -DCONFIG_IMAGE_RAW_DATA
	GLOBAL_CFLAGS += -DCONFIG_IMAGE_RAW_DATA
endif
ifeq ($(CONFIG_JPEG_EN),y)
	CPPFLAGS += -DCONFIG_JPEG
	GLOBAL_CFLAGS += -DCONFIG_JPEG
endif
#CPPFLAGS += -DCONFIG_SPEECH


vpath %.c $(DIR) $(shell find $(DIR) -type d)
vpath %.cpp $(DIR) $(shell find $(DIR) -type d)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC += $(DIR)/utils/src/widget/lv_ext/lv_anim_loading.c \
	$(DIR)/utils/src/image_decoder.c

CPPSRC += $(DIR)/utils/src/page/page_base.cpp \
	$(DIR)/utils/src/page/page_manager_base.cpp \
	$(DIR)/utils/src/page/page_manager_drag.cpp \
	$(DIR)/utils/src/page/page_manager_router.cpp \
	$(DIR)/utils/src/page/page_manager_anim.cpp \
	$(DIR)/utils/src/page/page_manager_state.cpp \
	$(DIR)/utils/src/resource/resource_manager.cpp \
	$(DIR)/utils/src/widget/lv_scrollview/lv_scrollview.cpp \
	$(DIR)/utils/src/widget/lv_scrollview/scroll_view.cpp

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))
OBJS += $(notdir $(CPPSRC:.cpp=.o))

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#
COPY_RAM_OBJS: CORE_TARGETS

lib: COPY_RAM_OBJS
	$(call GEN_AND_MOVE_TO_LIB_FOLDER, $(TARGET_LIB), $(TARGET_LIBAPP_FOLDER))

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

