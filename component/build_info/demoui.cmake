if (__INCLUDED_DEMOUI_BUILD_CMAKE__)
    return()
else()
    set(__INCLUDED_DEMOUI_BUILD_CMAKE__ TRUE)
endif()

add_library(src_${d_MCU_PROJECT_NAME}_demoui_app INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_demoui_app2 INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_demoui_entry INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_demoui_log INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_demoui_res INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_demoui_utils INTERFACE)

#--------------------------#
ameba_target_include(src_${d_MCU_PROJECT_NAME}_demoui_app p_SCOPE interface
    ${c_CMPT_UI_DIR}/LVGL/lvgl
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/core
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/draw
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/extra
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/font
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/hal
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/misc
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/widgets
    ${c_CMPT_UI_DIR}/third_party/libpng/interfaces/include
    ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/interfaces/include
    ${c_CMPT_UI_DIR}/third_party/zlib/interfaces/include

    ${c_CMPT_APPLICATION_DIR}/demoui
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui
    ${c_CMPT_APPLICATION_DIR}/demoui/utils/interfaces
    ${c_CMPT_APPLICATION_DIR}/demoui/utils/src/resource
    ${c_CMPT_UI_DIR}/LVGL/port/${d_PLATFORM_TYPE}
    ${c_CMPT_UI_DIR}/LVGL/lv_drivers/interfaces
)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_demoui_app p_SCOPE interface LV_LVGL_H_INCLUDE_SIMPLE)
ameba_target_definitions_if(CONFIG_IMAGE_LITTLEFS src_${d_MCU_PROJECT_NAME}_demoui_app p_SCOPE interface CONFIG_IMAGE_LITTLEFS)
ameba_target_definitions_if(CONFIG_IMAGE_SD_CARD src_${d_MCU_PROJECT_NAME}_demoui_app p_SCOPE interface CONFIG_IMAGE_SD_CARD)
ameba_target_definitions_if(CONFIG_IMAGE_RAW_DATA src_${d_MCU_PROJECT_NAME}_demoui_app p_SCOPE interface CONFIG_IMAGE_RAW_DATA)
ameba_target_definitions_if(CONFIG_JPEG_EN src_${d_MCU_PROJECT_NAME}_demoui_app p_SCOPE interface CONFIG_JPEG)
# ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_demoui_app p_SCOPE interface CONFIG_SPEECH)
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_demoui_app p_SCOPE interface
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/boot_animation/boot_animation.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/boot_animation/boot_animation_view.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/pages/launcher/launcher.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/pages/launcher/launcher_view.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/pages/launcher/scrollviews/home/home.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/pages/launcher/scrollviews/home/home_view.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/pages/launcher/scrollviews/home/home_model.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/pages/launcher/scrollviews/settings/settings.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/pages/launcher/scrollviews/settings/settings_view.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/pages/launcher/scrollviews/music/music.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/pages/launcher/scrollviews/music/music_view.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/pages/wifi_settings/wifi_settings.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/pages/wifi_settings/wifi_settings_view.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/pages/wifi_ap_settings/wifi_ap_settings.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/pages/wifi_ap_settings/wifi_ap_settings_view.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/pages/ota_settings/ota_settings.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/pages/ota_settings/ota_settings_view.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/pages/audio_settings/audio_settings_model.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/pages/audio_settings/audio_settings.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/pages/audio_settings/audio_settings_view.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/pages/advance/advance.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/pages/advance/advance_view.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/pages/reset/reset.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/pages/reset/reset_view.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/pages/reset/reset_model.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/system_ui/system_ui.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/system_ui/speech/speech_view.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/system_ui/speech/speech_presenter.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/system_ui/speech/speech_model.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/resource/resource_pool.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/services/music/music_service.cpp

    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/resource/image/image_music_btn_list_pause.c
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/resource/image/image_music_btn_list_play.c
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/resource/image/image_music_btn_loop.c
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/resource/image/image_music_btn_next.c
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/resource/image/image_music_btn_pause.c
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/resource/image/image_music_btn_prev.c
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/resource/image/image_music_btn_rnd.c
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/resource/image/image_music_btn_play.c
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/resource/image/image_music_slider_knob.c
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/resource/image/image_music_wave_top.c
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/resource/image/image_music_wave_bottom.c
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/resource/image/image_music_corner_left.c
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/resource/image/image_music_corner_right.c
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/resource/image/image_music_cover_1.c
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/resource/image/image_music_cover_2.c
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/resource/image/image_music_cover_3.c
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/resource/image/image_music_list_border.c
)

ameba_target_sources_if(CONFIG_IMAGE_RAW_DATA src_${d_MCU_PROJECT_NAME}_demoui_app p_SCOPE interface
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/resource/image/image_direction_left.c
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/resource/image/image_icon_wifi.c
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/resource/image/image_icon_audio.c
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/resource/image/image_icon_system_info.c
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/resource/image/image_ssid_wifi_strong.c
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/resource/image/image_ssid_wifi_lock_strong.c
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/resource/image/image_volume_mute.c
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/resource/image/image_volume_unmute.c
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/resource/image/image_ok.c
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/resource/image/image_close.c
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/resource/image/image_icon_home.c
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/resource/image/image_icon_info.c
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/resource/image/image_icon_music.c
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/resource/image/image_icon_settings.c
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui/resource/image/image_realtek_logo.c
)
#--------------------------#
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_demoui_app2 p_SCOPE interface LV_LVGL_H_INCLUDE_SIMPLE)
ameba_target_definitions_if(CONFIG_IMAGE_LITTLEFS src_${d_MCU_PROJECT_NAME}_demoui_app2 p_SCOPE interface CONFIG_IMAGE_LITTLEFS)
ameba_target_definitions_if(CONFIG_IMAGE_SD_CARD src_${d_MCU_PROJECT_NAME}_demoui_app2 p_SCOPE interface CONFIG_IMAGE_SD_CARD)
ameba_target_definitions_if(CONFIG_IMAGE_RAW_DATA src_${d_MCU_PROJECT_NAME}_demoui_app2 p_SCOPE interface CONFIG_IMAGE_RAW_DATA)
ameba_target_definitions_if(CONFIG_JPEG_EN src_${d_MCU_PROJECT_NAME}_demoui_app2 p_SCOPE interface CONFIG_JPEG)
# ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_demoui_app2 p_SCOPE interface CONFIG_SPEECH)

ameba_target_include(src_${d_MCU_PROJECT_NAME}_demoui_app2 p_SCOPE interface
    ${c_CMPT_UI_DIR}/LVGL/lvgl
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/core
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/draw
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/extra
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/font
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/hal
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/misc
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/widgets
    ${c_CMPT_UI_DIR}/third_party/libpng/interfaces/include
    ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/interfaces/include
    ${c_CMPT_UI_DIR}/third_party/zlib/interfaces/include

    ${c_CMPT_APPLICATION_DIR}/demoui
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui
    ${c_CMPT_APPLICATION_DIR}/demoui/utils/interfaces
    ${c_CMPT_APPLICATION_DIR}/demoui/utils/src/resource
    ${c_CMPT_UI_DIR}/LVGL/port/${d_PLATFORM_TYPE}
    ${c_CMPT_UI_DIR}/LVGL/lv_drivers/interfaces
)

ameba_target_sources(src_${d_MCU_PROJECT_NAME}_demoui_app2 p_SCOPE interface
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/lv_iot_gui.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/account_settings/account_settings.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/account_settings/account_settings_view.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/clock_detail/clock_detail.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/clock_detail/clock_detail_view.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/clock_ringtone/clock_ringtone.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/clock_ringtone/clock_ringtone_view.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/display_settings/display_settings.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/display_settings/display_settings_view.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/guide_settings/guide_settings.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/guide_settings/guide_settings_view.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/individuation_settings/individuation_settings.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/individuation_settings/individuation_settings_view.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/launcher/launcher.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/launcher/launcher_view.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/launcher/scrollviews/air_condition/air_condition.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/launcher/scrollviews/air_condition/air_condition_view.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/launcher/scrollviews/clock/clock.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/launcher/scrollviews/clock/clock_view.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/launcher/scrollviews/home/home.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/launcher/scrollviews/home/home_view.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/launcher/scrollviews/home/home_model.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/launcher/scrollviews/light/light.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/launcher/scrollviews/light/light_view.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/launcher/scrollviews/settings/settings.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/launcher/scrollviews/settings/settings_view.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/light_settings/light_settings.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/light_settings/light_settings_view.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/light_settings_add_room/light_settings_add_room.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/light_settings_add_room/light_settings_add_room_view.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/light_settings_add_timer/light_settings_add_timer.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/light_settings_add_timer/light_settings_add_timer_view.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/light_settings_add_timer_select/light_settings_add_timer_select.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/light_settings_add_timer_select/light_settings_add_timer_select_view.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/light_settings_device_name/light_settings_device_name.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/light_settings_device_name/light_settings_device_name_view.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/light_settings_device_type/light_settings_device_type.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/light_settings_device_type/light_settings_device_type_view.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/light_settings_location/light_settings_location.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/light_settings_location/light_settings_location_view.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/light_settings_support_cmd/light_settings_support_cmd.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/light_settings_support_cmd/light_settings_support_cmd_view.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/light_settings_timer/light_settings_timer.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/light_settings_timer/light_settings_timer_view.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/speech_air_condition_guide_settings/speech_air_condition_guide_settings.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/speech_air_condition_guide_settings/speech_air_condition_guide_settings_view.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/speech_guide_settings/speech_guide_settings.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/speech_guide_settings/speech_guide_settings_view.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/speech_light_guide_settings/speech_light_guide_settings.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/speech_light_guide_settings/speech_light_guide_settings_view.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/speech_settings/speech_settings.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/speech_settings/speech_settings_view.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/voice_settings/voice_settings.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/voice_settings/voice_settings_view.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/wifi_ap_settings/wifi_ap_settings.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/wifi_ap_settings/wifi_ap_settings_view.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/wifi_settings/wifi_settings.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/wifi_settings/wifi_settings_view.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/system_ui/system_ui.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/system_ui/menu/menu.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/pages/system_ui/menu/menu_view.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/resource/resource_pool.cpp

    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/resource/font/su_15.c
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/resource/font/su_16.c
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/resource/font/su_17.c
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/resource/font/su_20.c
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/resource/font/su_22.c
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/resource/font/su_30.c
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/resource/font/su_50.c
    ${c_CMPT_APPLICATION_DIR}/demoui/app2/ui/resource/font/su_94.c
)
#--------------------------#
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_demoui_utils p_SCOPE interface LV_LVGL_H_INCLUDE_SIMPLE)
ameba_target_definitions_if(CONFIG_IMAGE_LITTLEFS src_${d_MCU_PROJECT_NAME}_demoui_utils p_SCOPE interface CONFIG_IMAGE_LITTLEFS)
ameba_target_definitions_if(CONFIG_IMAGE_SD_CARD src_${d_MCU_PROJECT_NAME}_demoui_utils p_SCOPE interface CONFIG_IMAGE_SD_CARD)
ameba_target_definitions_if(CONFIG_IMAGE_RAW_DATA src_${d_MCU_PROJECT_NAME}_demoui_utils p_SCOPE interface CONFIG_IMAGE_RAW_DATA)
ameba_target_definitions_if(CONFIG_JPEG_EN src_${d_MCU_PROJECT_NAME}_demoui_utils p_SCOPE interface CONFIG_JPEG)
# ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_demoui_utils p_SCOPE interface CONFIG_SPEECH)

ameba_target_include(src_${d_MCU_PROJECT_NAME}_demoui_utils p_SCOPE interface
    ${c_CMPT_UI_DIR}/LVGL/lvgl
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/core
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/draw
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/extra
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/font
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/hal
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/misc
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/widgets
    ${c_CMPT_UI_DIR}/third_party/libpng/interfaces/include
    ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/interfaces/include
    ${c_CMPT_UI_DIR}/third_party/zlib/interfaces/include

    ${c_CMPT_APPLICATION_DIR}/demoui
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui
    ${c_CMPT_APPLICATION_DIR}/demoui/utils/interfaces
    ${c_CMPT_APPLICATION_DIR}/demoui/utils/src/resource
    ${c_CMPT_UI_DIR}/LVGL/port/${d_PLATFORM_TYPE}
    ${c_CMPT_UI_DIR}/LVGL/lv_drivers/interfaces
)
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_demoui_utils p_SCOPE interface
    ${c_CMPT_APPLICATION_DIR}/demoui/utils/src/widget/lv_ext/lv_anim_loading.c
    ${c_CMPT_APPLICATION_DIR}/demoui/utils/src/image_decoder.c
    ${c_CMPT_APPLICATION_DIR}/demoui/utils/src/page/page_base.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/utils/src/page/page_manager_base.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/utils/src/page/page_manager_drag.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/utils/src/page/page_manager_router.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/utils/src/page/page_manager_anim.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/utils/src/page/page_manager_state.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/utils/src/resource/resource_manager.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/utils/src/widget/lv_scrollview/lv_scrollview.cpp
    ${c_CMPT_APPLICATION_DIR}/demoui/utils/src/widget/lv_scrollview/scroll_view.cpp
)
#--------------------------#
ameba_target_include(src_${d_MCU_PROJECT_NAME}_demoui_entry p_SCOPE interface
    ${c_CMPT_UI_DIR}/LVGL/lvgl
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/core
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/draw
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/extra
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/font
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/hal
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/misc
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/widgets
    ${c_CMPT_UI_DIR}/third_party/libpng/interfaces/include
    ${c_CMPT_UI_DIR}/third_party/libjpeg-turbo/interfaces/include
    ${c_CMPT_UI_DIR}/third_party/zlib/interfaces/include

    ${c_CMPT_APPLICATION_DIR}/demoui
    ${c_CMPT_APPLICATION_DIR}/demoui/app/ui
    ${c_CMPT_APPLICATION_DIR}/demoui/utils/interfaces
    ${c_CMPT_APPLICATION_DIR}/demoui/utils/src/resource
    ${c_CMPT_UI_DIR}/LVGL/port/${d_PLATFORM_TYPE}
    ${c_CMPT_UI_DIR}/LVGL/lv_drivers/interfaces
    ${c_CMPT_UI_DIR}/LVGL/lvgl/demos/stress
)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_demoui_entry p_SCOPE interface
    LV_LVGL_H_INCLUDE_SIMPLE
)
ameba_target_definitions_if(CONFIG_IMAGE_SD_CARD src_${d_MCU_PROJECT_NAME}_demoui_entry p_SCOPE interface CONFIG_IMAGE_SD_CARD)

ameba_target_sources(src_${d_MCU_PROJECT_NAME}_demoui_entry p_SCOPE interface
    ${c_CMPT_APPLICATION_DIR}/demoui/entry/demoui_example.c
)
#--------------------------#