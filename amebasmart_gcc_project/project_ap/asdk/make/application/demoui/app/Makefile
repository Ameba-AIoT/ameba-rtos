
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/application/demoui
DIRLV = $(BASEDIR)/component/ui/LVGL/lvgl
DIRAPP = $(BASEDIR)/component/application/demoui/app

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


vpath %.c $(DIRAPP) $(shell find $(DIRAPP) -type d)
vpath %.cpp $(DIRAPP) $(shell find $(DIRAPP) -type d)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CPPSRC += $(DIR)/app/ui/lv_pangu_gui.cpp \
	$(DIR)/app/ui/boot_animation/boot_animation.cpp \
	$(DIR)/app/ui/boot_animation/boot_animation_view.cpp \
	$(DIR)/app/ui/pages/launcher/launcher.cpp \
	$(DIR)/app/ui/pages/launcher/launcher_view.cpp \
	$(DIR)/app/ui/pages/launcher/scrollviews/home/home.cpp \
	$(DIR)/app/ui/pages/launcher/scrollviews/home/home_view.cpp \
	$(DIR)/app/ui/pages/launcher/scrollviews/home/home_model.cpp \
	$(DIR)/app/ui/pages/launcher/scrollviews/settings/settings.cpp \
	$(DIR)/app/ui/pages/launcher/scrollviews/settings/settings_view.cpp \
	$(DIR)/app/ui/pages/launcher/scrollviews/music/music.cpp \
	$(DIR)/app/ui/pages/launcher/scrollviews/music/music_view.cpp \
	$(DIR)/app/ui/pages/wifi_settings/wifi_settings.cpp \
	$(DIR)/app/ui/pages/wifi_settings/wifi_settings_view.cpp \
	$(DIR)/app/ui/pages/wifi_ap_settings/wifi_ap_settings.cpp \
	$(DIR)/app/ui/pages/wifi_ap_settings/wifi_ap_settings_view.cpp \
	$(DIR)/app/ui/pages/ota_settings/ota_settings.cpp \
	$(DIR)/app/ui/pages/ota_settings/ota_settings_view.cpp \
	$(DIR)/app/ui/pages/audio_settings/audio_settings_model.cpp \
	$(DIR)/app/ui/pages/audio_settings/audio_settings.cpp \
	$(DIR)/app/ui/pages/audio_settings/audio_settings_view.cpp \
	$(DIR)/app/ui/pages/advance/advance.cpp \
	$(DIR)/app/ui/pages/advance/advance_view.cpp \
	$(DIR)/app/ui/pages/reset/reset.cpp \
	$(DIR)/app/ui/pages/reset/reset_view.cpp \
	$(DIR)/app/ui/pages/reset/reset_model.cpp \
	$(DIR)/app/ui/system_ui/system_ui.cpp \
	$(DIR)/app/ui/system_ui/speech/speech_view.cpp \
	$(DIR)/app/ui/system_ui/speech/speech_presenter.cpp \
	$(DIR)/app/ui/system_ui/speech/speech_model.cpp \
	$(DIR)/app/ui/resource/resource_pool.cpp \
	$(DIR)/app/ui/services/music/music_service.cpp

CSRC += $(DIR)/app/ui/resource/image/image_music_btn_list_pause.c \
	$(DIR)/app/ui/resource/image/image_music_btn_list_play.c \
	$(DIR)/app/ui/resource/image/image_music_btn_loop.c \
	$(DIR)/app/ui/resource/image/image_music_btn_next.c \
	$(DIR)/app/ui/resource/image/image_music_btn_pause.c \
	$(DIR)/app/ui/resource/image/image_music_btn_prev.c \
	$(DIR)/app/ui/resource/image/image_music_btn_rnd.c \
	$(DIR)/app/ui/resource/image/image_music_btn_play.c \
	$(DIR)/app/ui/resource/image/image_music_slider_knob.c \
	$(DIR)/app/ui/resource/image/image_music_wave_top.c \
	$(DIR)/app/ui/resource/image/image_music_wave_bottom.c \
	$(DIR)/app/ui/resource/image/image_music_corner_left.c \
	$(DIR)/app/ui/resource/image/image_music_corner_right.c \
	$(DIR)/app/ui/resource/image/image_music_cover_1.c \
	$(DIR)/app/ui/resource/image/image_music_cover_2.c \
	$(DIR)/app/ui/resource/image/image_music_cover_3.c \
	$(DIR)/app/ui/resource/image/image_music_list_border.c \

ifeq ($(CONFIG_IMAGE_RAW_DATA),y)
CSRC += $(DIR)/app/ui/resource/image/image_bg.c \
	$(DIR)/app/ui/resource/image/image_direction_left.c \
	$(DIR)/app/ui/resource/image/image_icon_wifi.c \
	$(DIR)/app/ui/resource/image/image_icon_audio.c \
	$(DIR)/app/ui/resource/image/image_icon_system_info.c \
	$(DIR)/app/ui/resource/image/image_ssid_wifi_strong.c \
	$(DIR)/app/ui/resource/image/image_ssid_wifi_lock_strong.c \
	$(DIR)/app/ui/resource/image/image_volume_mute.c \
	$(DIR)/app/ui/resource/image/image_volume_unmute.c \
	$(DIR)/app/ui/resource/image/image_ok.c \
	$(DIR)/app/ui/resource/image/image_close.c \
	$(DIR)/app/ui/resource/image/image_icon_home.c \
	$(DIR)/app/ui/resource/image/image_icon_info.c \
	$(DIR)/app/ui/resource/image/image_icon_music.c \
	$(DIR)/app/ui/resource/image/image_icon_settings.c \
	$(DIR)/app/ui/resource/image/image_realtek_logo.c
endif

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

