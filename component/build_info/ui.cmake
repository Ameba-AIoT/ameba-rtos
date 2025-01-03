if (__INCLUDED_UI_BUILD_CMAKE__)
    return()
else()
    set(__INCLUDED_UI_BUILD_CMAKE__ TRUE)
endif()

add_library(src_${d_MCU_PROJECT_NAME}_ui_drivers_cst328 INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_ui_drivers_st7701s INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_ui_drivers_xip INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_ui_lvgl INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_ui_lvgl_drivers INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_ui_lvgl_test INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_ui_emwin_library INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_ui_emwin_sample INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_ui_emwin_third INTERFACE)

#--------------------------#
ameba_target_include(src_${d_MCU_PROJECT_NAME}_ui_drivers_cst328 p_SCOPE interface
    ${c_CMPT_UI_DIR}/drivers/cst328/include
)
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_ui_drivers_cst328 p_SCOPE interface
    ${c_CMPT_UI_DIR}/drivers/cst328/src/cst328.c
)
#--------------------------#
ameba_target_include(src_${d_MCU_PROJECT_NAME}_ui_drivers_st7701s p_SCOPE interface
    ${c_CMPT_UI_DIR}/drivers/st7701s/include
)
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_ui_drivers_st7701s p_SCOPE interface
    ${c_CMPT_UI_DIR}/drivers/st7701s/src/st7701s.c
)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_ui_drivers_st7701s p_SCOPE interface -UDEBUG -Wno-int-conversion)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_ui_drivers_st7701s p_SCOPE interface __RTOS__ NDEBUG)

#--------------------------#
ameba_target_include(src_${d_MCU_PROJECT_NAME}_ui_lvgl p_SCOPE interface
    ${c_CMPT_UI_DIR}/LVGL/port/amebasmart
    ${c_CMPT_UI_DIR}/LVGL/lvgl
    ${c_CMPT_UI_DIR}/LVGL/lvgl/
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/core
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/draw
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/extra
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/font
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/hal
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/misc
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/widgets
)
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_ui_lvgl p_SCOPE interface
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/core/lv_disp.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/core/lv_event.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/core/lv_group.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/core/lv_indev.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/core/lv_indev_scroll.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/core/lv_obj.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/core/lv_obj_class.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/core/lv_obj_draw.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/core/lv_obj_pos.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/core/lv_obj_scroll.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/core/lv_obj_style.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/core/lv_obj_style_gen.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/core/lv_obj_tree.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/core/lv_refr.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/core/lv_theme.c

    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/draw/lv_draw_arc.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/draw/lv_draw.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/draw/lv_draw_img.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/draw/lv_draw_label.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/draw/lv_draw_line.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/draw/lv_draw_mask.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/draw/lv_draw_rect.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/draw/lv_draw_transform.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/draw/lv_draw_layer.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/draw/lv_draw_triangle.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/draw/lv_img_buf.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/draw/lv_img_cache.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/draw/lv_img_decoder.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/draw/sw/lv_draw_sw.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/draw/sw/lv_draw_sw_arc.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/draw/sw/lv_draw_sw_blend.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/draw/sw/lv_draw_sw_dither.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/draw/sw/lv_draw_sw_gradient.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/draw/sw/lv_draw_sw_img.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/draw/sw/lv_draw_sw_letter.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/draw/sw/lv_draw_sw_line.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/draw/sw/lv_draw_sw_polygon.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/draw/sw/lv_draw_sw_rect.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/draw/sw/lv_draw_sw_transform.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/draw/sw/lv_draw_sw_layer.c

    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/extra/lv_extra.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/extra/layouts/flex/lv_flex.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/extra/layouts/grid/lv_grid.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/extra/themes/basic/lv_theme_basic.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/extra/themes/default/lv_theme_default.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/extra/themes/mono/lv_theme_mono.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/extra/widgets/animimg/lv_animimg.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/extra/widgets/chart/lv_chart.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/extra/widgets/imgbtn/lv_imgbtn.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/extra/widgets/led/lv_led.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/extra/widgets/msgbox/lv_msgbox.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/extra/widgets/spinbox/lv_spinbox.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/extra/widgets/tabview/lv_tabview.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/extra/widgets/win/lv_win.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/extra/widgets/calendar/lv_calendar.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/extra/widgets/calendar/lv_calendar_header_arrow.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/extra/widgets/calendar/lv_calendar_header_dropdown.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/extra/widgets/colorwheel/lv_colorwheel.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/extra/widgets/keyboard/lv_keyboard.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/extra/widgets/list/lv_list.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/extra/widgets/menu/lv_menu.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/extra/widgets/meter/lv_meter.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/extra/widgets/span/lv_span.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/extra/widgets/spinner/lv_spinner.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/extra/widgets/tileview/lv_tileview.c

    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/font/lv_extra.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/font/lv_font.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/font/lv_font_dejavu_16_persian_hebrew.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/font/lv_font_fmt_txt.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/font/lv_font_loader.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/font/lv_font_montserrat_10.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/font/lv_font_montserrat_12.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/font/lv_font_montserrat_12_subpx.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/font/lv_font_montserrat_14.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/font/lv_font_montserrat_16.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/font/lv_font_montserrat_18.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/font/lv_font_montserrat_20.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/font/lv_font_montserrat_22.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/font/lv_font_montserrat_24.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/font/lv_font_montserrat_26.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/font/lv_font_montserrat_28.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/font/lv_font_montserrat_28_compressed.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/font/lv_font_montserrat_30.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/font/lv_font_montserrat_32.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/font/lv_font_montserrat_34.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/font/lv_font_montserrat_36.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/font/lv_font_montserrat_38.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/font/lv_font_montserrat_40.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/font/lv_font_montserrat_42.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/font/lv_font_montserrat_44.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/font/lv_font_montserrat_46.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/font/lv_font_montserrat_48.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/font/lv_font_montserrat_8.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/font/lv_font_simsun_16_cjk.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/font/lv_font_unscii_16.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/font/lv_font_unscii_8.c

    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/hal/lv_hal_disp.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/hal/lv_hal_indev.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/hal/lv_hal_tick.c

    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/misc/lv_anim.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/misc/lv_anim_timeline.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/misc/lv_area.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/misc/lv_async.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/misc/lv_bidi.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/misc/lv_color.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/misc/lv_fs.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/misc/lv_gc.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/misc/lv_ll.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/misc/lv_log.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/misc/lv_math.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/misc/lv_mem.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/misc/lv_printf.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/misc/lv_style.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/misc/lv_style_gen.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/misc/lv_timer.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/misc/lv_tlsf.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/misc/lv_txt_ap.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/misc/lv_txt.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/misc/lv_utils.c

    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/widgets/lv_arc.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/widgets/lv_bar.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/widgets/lv_btn.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/widgets/lv_btnmatrix.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/widgets/lv_canvas.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/widgets/lv_checkbox.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/widgets/lv_dropdown.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/widgets/lv_img.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/widgets/lv_label.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/widgets/lv_line.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/widgets/lv_roller.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/widgets/lv_slider.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/widgets/lv_switch.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/widgets/lv_table.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/widgets/lv_textarea.c

    ${c_CMPT_UI_DIR}/LVGL/lvgl/demos/stress/lv_demo_stress.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/demos/benchmark/lv_demo_benchmark.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/demos/benchmark/img_benchmark_cogwheel_argb.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/demos/benchmark/img_benchmark_cogwheel_rgb.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/demos/benchmark/img_benchmark_cogwheel_chroma_keyed.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/demos/benchmark/img_benchmark_cogwheel_indexed16.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/demos/benchmark/img_benchmark_cogwheel_alpha16.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/demos/benchmark/lv_font_bechmark_montserrat_12_compr_az.c.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/demos/benchmark/lv_font_bechmark_montserrat_16_compr_az.c.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/demos/benchmark/lv_font_bechmark_montserrat_28_compr_az.c.c

    ${c_CMPT_UI_DIR}/LVGL/lvgl/demos/widgets/lv_demo_widgets.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/demos/widgets/asserts/img_clothes.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/demos/widgets/asserts/img_demo_widgets_avatar.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/demos/widgets/asserts/img_lvgl_logo.c

    ${c_CMPT_UI_DIR}/LVGL/lvgl/examples/anim/lv_example_anim_1.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/examples/anim/lv_example_anim_2.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/examples/anim/lv_example_anim_3.c
    ${c_CMPT_UI_DIR}/LVGL/lvgl/examples/anim/lv_example_anim_timeline_1.c
)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_ui_lvgl p_SCOPE interface LV_CONF_INCLUDE_SIMPLE)

#--------------------------#
ameba_target_include(src_${d_MCU_PROJECT_NAME}_ui_lvgl_drivers p_SCOPE interface
    ${c_CMPT_UI_DIR}/LVGL/lv_drivers/interfaces
    ${c_CMPT_UI_DIR}/drivers/cst328/include
    ${c_CMPT_UI_DIR}/drivers/st7701s/include
    ${c_CMPT_UI_DIR}/LVGL/port/amebasmart
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/core
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/draw
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/extra
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/font
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/hal
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/misc
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/widgets
)
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_ui_lvgl_drivers p_SCOPE interface

    ${c_CMPT_UI_DIR}/LVGL/lv_drivers/${d_PLATFORM_TYPE}/src/display.c
    ${c_CMPT_UI_DIR}/LVGL/lv_drivers/${d_PLATFORM_TYPE}/src/touch.c
    ${c_CMPT_UI_DIR}/LVGL/lv_drivers/${d_PLATFORM_TYPE}/src/lv_hal.c
)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_ui_lvgl_drivers p_SCOPE interface -UDEBUG -Wno-incompatible-pointer-types)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_ui_lvgl_drivers p_SCOPE interface __RTOS__ NDEBUG)

#--------------------------#
ameba_target_include(src_${d_MCU_PROJECT_NAME}_ui_lvgl_test p_SCOPE interface
    ${c_CMPT_UI_DIR}/LVGL/lvgl
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/core
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/draw
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/extra
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/font
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/hal
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/misc
    ${c_CMPT_UI_DIR}/LVGL/lvgl/src/widgets

    ${c_CMPT_UI_DIR}/LVGL/lv_drivers/${d_PLATFORM_TYPE}/test
    ${c_CMPT_UI_DIR}/LVGL/port/${d_PLATFORM_TYPE}
    ${c_CMPT_UI_DIR}/LVGL/lv_drivers/interfaces
)
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_ui_lvgl_test p_SCOPE interface
    ${c_CMPT_UI_DIR}/LVGL/lv_drivers/${d_PLATFORM_TYPE}/test/touch_example.c
)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_ui_lvgl_test p_SCOPE interface
    LV_LVGL_H_INCLUDE_SIMPLE
    LV_CONF_INCLUDE_SIMPLE
)

#--------------------------#

ameba_target_include(src_${d_MCU_PROJECT_NAME}_ui_emwin_library p_SCOPE interface
    ${c_CMPT_UI_DIR}/emWin/emwin/emWinLibrary/GUI
    ${c_CMPT_UI_DIR}/emWin/emwin/emWinLibrary/GUI_X
    ${c_CMPT_UI_DIR}/emWin/emwin/Sample/rtl8721d/Config
    ${c_CMPT_UI_DIR}/emWin/emwin/Sample/rtl8721d/hal
)
# set(_tmp_emwin_src_dir
#     AntiAlias
#     ConvertColor
#     ConvertMono
#     Core
#     DisplayDriver
#     Font
#     MemDev
#     TouchDriver
#     Widget
#     WM
# )
# foreach(SUBDIR IN LISTS _tmp_emwin_src_dir)
#     file(GLOB _tmp_src_files ${c_CMPT_UI_DIR}/emWin/emwin/emWinLibrary/GUI/${SUBDIR}/*.c)
#     ameba_target_sources(src_${d_MCU_PROJECT_NAME}_ui_emwin_library p_SCOPE interface ${_tmp_src_files})
# endforeach()
# unset(_tmp_emwin_src_dir)

ameba_target_sources(src_${d_MCU_PROJECT_NAME}_ui_emwin_library p_SCOPE interface
    ${c_CMPT_UI_DIR}/emWin/emwin/emWinLibrary/GUI_X/GUI_X_FreeRTOS.c
    ${c_CMPT_UI_DIR}/emWin/emwin/emWinLibrary/GUI_X/GUI_X_Touch_Analog.c
)
#--------------------------#
ameba_target_include(src_${d_MCU_PROJECT_NAME}_ui_emwin_sample p_SCOPE interface
    ${c_CMPT_UI_DIR}/emWin/emwin/Sample/rtl8721d
    ${c_CMPT_UI_DIR}/emWin/emwin/Sample/rtl8721d/Config
    ${c_CMPT_UI_DIR}/emWin/emwin/Sample/rtl8721d/hal
)
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_ui_emwin_sample p_SCOPE interface
    ${c_CMPT_UI_DIR}/emWin/emwin/Sample/rtl8721d/Config/GUIConf.c
    ${c_CMPT_UI_DIR}/emWin/emwin/Sample/rtl8721d/hal/hal_touch.c
    ${c_CMPT_UI_DIR}/emWin/emwin/Sample/rtl8721d/hal/gt911.c
    ${c_CMPT_UI_DIR}/emWin/emwin/Sample/rtl8721d/hal/hal_rgb_lcd.c
    ${c_CMPT_UI_DIR}/emWin/emwin/Sample/rtl8721d/Config/LCDConf_RGB_6bit_eval.c
    # ${c_CMPT_UI_DIR}/emWin/emwin/Sample/rtl8721d/hal/ft6236.c
    # ${c_CMPT_UI_DIR}/emWin/emwin/Sample/rtl8721d/hal/hal_mcu_lcd.c
    # ${c_CMPT_UI_DIR}/emWin/emwin/Sample/rtl8721d/Config/LCDConf_MCU_8bit_eval.c
)
#--------------------------#
ameba_target_include(src_${d_MCU_PROJECT_NAME}_ui_emwin_third p_SCOPE interface
    ${c_CMPT_UI_DIR}/emWin/emwin/Third_Party/truetype
    ${c_CMPT_UI_DIR}/emWin/emwin/Sample/rtl8721d/Config
    ${c_CMPT_UI_DIR}/emWin/emwin/Sample/rtl8721d/hal
)
file(GLOB _tmp_src_files ${c_CMPT_UI_DIR}/emWin/emwin/Third_Party/truetype/*.c)
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_ui_emwin_third p_SCOPE interface ${_tmp_src_files})

#--------------------------#