
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIRDEMUI = $(BASEDIR)/component/application/demoui
DIRLV = $(BASEDIR)/component/ui/LVGL/lvgl
DIR = $(BASEDIR)/component/application/demoui/app2/ui

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

MODULE_IFLAGS +=  -I$(DIRDEMUI) \
	-I$(DIRDEMUI)/utils/interfaces \
	-I$(DIRDEMUI)/utils/src/resource \
	-I$(BASEDIR)/component/ui/LVGL \
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
CPPSRC += $(DIR)/lv_iot_gui.cpp \
	$(DIR)/pages/account_settings/account_settings.cpp \
	$(DIR)/pages/account_settings/account_settings_view.cpp \
	$(DIR)/pages/clock_detail/clock_detail.cpp \
	$(DIR)/pages/clock_detail/clock_detail_view.cpp \
	$(DIR)/pages/clock_ringtone/clock_ringtone.cpp \
	$(DIR)/pages/clock_ringtone/clock_ringtone_view.cpp \
	$(DIR)/pages/display_settings/display_settings.cpp \
	$(DIR)/pages/display_settings/display_settings_view.cpp \
	$(DIR)/pages/guide_settings/guide_settings.cpp \
	$(DIR)/pages/guide_settings/guide_settings_view.cpp \
	$(DIR)/pages/individuation_settings/individuation_settings.cpp \
	$(DIR)/pages/individuation_settings/individuation_settings_view.cpp \
	$(DIR)/pages/launcher/launcher.cpp \
	$(DIR)/pages/launcher/launcher_view.cpp \
	$(DIR)/pages/launcher/scrollviews/air_condition/air_condition.cpp \
	$(DIR)/pages/launcher/scrollviews/air_condition/air_condition_view.cpp \
	$(DIR)/pages/launcher/scrollviews/clock/clock.cpp \
	$(DIR)/pages/launcher/scrollviews/clock/clock_view.cpp \
	$(DIR)/pages/launcher/scrollviews/home/home.cpp \
	$(DIR)/pages/launcher/scrollviews/home/home_view.cpp \
	$(DIR)/pages/launcher/scrollviews/home/home_model.cpp \
	$(DIR)/pages/launcher/scrollviews/light/light.cpp \
	$(DIR)/pages/launcher/scrollviews/light/light_view.cpp \
	$(DIR)/pages/launcher/scrollviews/settings/settings.cpp \
	$(DIR)/pages/launcher/scrollviews/settings/settings_view.cpp \
	$(DIR)/pages/light_settings/light_settings.cpp \
	$(DIR)/pages/light_settings/light_settings_view.cpp \
	$(DIR)/pages/light_settings_add_room/light_settings_add_room.cpp \
	$(DIR)/pages/light_settings_add_room/light_settings_add_room_view.cpp \
	$(DIR)/pages/light_settings_add_timer/light_settings_add_timer.cpp \
	$(DIR)/pages/light_settings_add_timer/light_settings_add_timer_view.cpp \
	$(DIR)/pages/light_settings_add_timer_select/light_settings_add_timer_select.cpp \
	$(DIR)/pages/light_settings_add_timer_select/light_settings_add_timer_select_view.cpp \
	$(DIR)/pages/light_settings_device_name/light_settings_device_name.cpp \
	$(DIR)/pages/light_settings_device_name/light_settings_device_name_view.cpp \
	$(DIR)/pages/light_settings_device_type/light_settings_device_type.cpp \
	$(DIR)/pages/light_settings_device_type/light_settings_device_type_view.cpp \
	$(DIR)/pages/light_settings_location/light_settings_location.cpp \
	$(DIR)/pages/light_settings_location/light_settings_location_view.cpp \
	$(DIR)/pages/light_settings_support_cmd/light_settings_support_cmd.cpp \
	$(DIR)/pages/light_settings_support_cmd/light_settings_support_cmd_view.cpp \
	$(DIR)/pages/light_settings_timer/light_settings_timer.cpp \
	$(DIR)/pages/light_settings_timer/light_settings_timer_view.cpp \
	$(DIR)/pages/speech_air_condition_guide_settings/speech_air_condition_guide_settings.cpp \
	$(DIR)/pages/speech_air_condition_guide_settings/speech_air_condition_guide_settings_view.cpp \
	$(DIR)/pages/speech_guide_settings/speech_guide_settings.cpp \
	$(DIR)/pages/speech_guide_settings/speech_guide_settings_view.cpp \
	$(DIR)/pages/speech_light_guide_settings/speech_light_guide_settings.cpp \
	$(DIR)/pages/speech_light_guide_settings/speech_light_guide_settings_view.cpp \
	$(DIR)/pages/speech_settings/speech_settings.cpp \
	$(DIR)/pages/speech_settings/speech_settings_view.cpp \
	$(DIR)/pages/voice_settings/voice_settings.cpp \
	$(DIR)/pages/voice_settings/voice_settings_view.cpp \
	$(DIR)/pages/wifi_ap_settings/wifi_ap_settings.cpp \
	$(DIR)/pages/wifi_ap_settings/wifi_ap_settings_view.cpp \
	$(DIR)/pages/wifi_settings/wifi_settings.cpp \
	$(DIR)/pages/wifi_settings/wifi_settings_view.cpp \
	$(DIR)/pages/system_ui/system_ui.cpp \
	$(DIR)/pages/system_ui/menu/menu.cpp \
	$(DIR)/pages/system_ui/menu/menu_view.cpp \
	$(DIR)/resource/resource_pool.cpp \

CSRC += $(DIR)/resource/font/su_15.c \
	$(DIR)/resource/font/su_16.c \
	$(DIR)/resource/font/su_17.c \
	$(DIR)/resource/font/su_20.c \
	$(DIR)/resource/font/su_22.c \
	$(DIR)/resource/font/su_30.c \
	$(DIR)/resource/font/su_50.c \
	$(DIR)/resource/font/su_94.c \


#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))
OBJS += $(notdir $(CPPSRC:.cpp=.o))

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

