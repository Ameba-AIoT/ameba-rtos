##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin
ameba_list_append(public_includes
    ${CMAKE_CURRENT_SOURCE_DIR}/lvgl/src
    ${CMAKE_CURRENT_SOURCE_DIR}/lvgl/src/core
    ${CMAKE_CURRENT_SOURCE_DIR}/lvgl/src/display
    ${CMAKE_CURRENT_SOURCE_DIR}/lvgl/src/draw
    ${CMAKE_CURRENT_SOURCE_DIR}/lvgl/src/drivers
    ${CMAKE_CURRENT_SOURCE_DIR}/lvgl/src/font
    ${CMAKE_CURRENT_SOURCE_DIR}/lvgl/src/indev
    ${CMAKE_CURRENT_SOURCE_DIR}/lvgl/src/layouts
    ${CMAKE_CURRENT_SOURCE_DIR}/lvgl/src/misc
    ${CMAKE_CURRENT_SOURCE_DIR}/lvgl/src/osal
    ${CMAKE_CURRENT_SOURCE_DIR}/lvgl/src/stdlib
    ${CMAKE_CURRENT_SOURCE_DIR}/lvgl/src/themes
    ${CMAKE_CURRENT_SOURCE_DIR}/lvgl/src/tick
    ${CMAKE_CURRENT_SOURCE_DIR}/lvgl/src/widgets
    ${CMAKE_CURRENT_SOURCE_DIR}/lvgl/demos/widgets
    ${CMAKE_CURRENT_SOURCE_DIR}/lvgl/demos/stress
    ${CMAKE_CURRENT_SOURCE_DIR}/lvgl/demos/benchmark
    ${CMAKE_CURRENT_SOURCE_DIR}/port/${c_SOC_TYPE}
)

# Component public part, user config end
#----------------------------------------#

#WARNING: Fixed section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_definitions})
ameba_global_library(${public_libraries}) #default: whole-archived

##########################################################################################
## * This part defines private part of the component
## * Private part is used to build target of current component
## * NOTE: The build API guarantees the global build configures(mentioned above)
## *       applied to the target automatically. So if any configure was already added
## *       to public above, it's unnecessary to add again below.

#NOTE: User defined section, add your private build configures here
# You may use if-else condition to set these predefined variable
# They are only for ameba_add_internal_library/ameba_add_external_app_library/ameba_add_external_soc_library
set(private_sources)                 #private source files, NOTE: relative path is OK
set(private_includes)                #private include directories, NOTE: relative path is OK
set(private_definitions)             #private definitions
set(private_compile_options)         #private compile_options

#------------------------------#
# Component private part, user config begin

ameba_list_append(private_sources
    lvgl/src/lv_init.c
    lvgl/src/core/lv_group.c
    lvgl/src/core/lv_obj.c
    lvgl/src/core/lv_obj_class.c
    lvgl/src/core/lv_obj_draw.c
    lvgl/src/core/lv_obj_event.c
    lvgl/src/core/lv_obj_id_builtin.c
    lvgl/src/core/lv_obj_pos.c
    lvgl/src/core/lv_obj_property.c
    lvgl/src/core/lv_obj_scroll.c
    lvgl/src/core/lv_obj_style.c
    lvgl/src/core/lv_obj_style_gen.c
    lvgl/src/core/lv_obj_tree.c
    lvgl/src/core/lv_refr.c

    lvgl/src/display/lv_display.c

    lvgl/src/draw/lv_draw_arc.c
    lvgl/src/draw/lv_draw_buf.c
    lvgl/src/draw/lv_draw.c
    lvgl/src/draw/lv_draw_image.c
    lvgl/src/draw/lv_draw_label.c
    lvgl/src/draw/lv_draw_line.c
    lvgl/src/draw/lv_draw_mask.c
    lvgl/src/draw/lv_draw_rect.c
    lvgl/src/draw/lv_draw_triangle.c
    lvgl/src/draw/lv_draw_vector.c
    lvgl/src/draw/lv_image_decoder.c
    lvgl/src/draw/sw/lv_draw_sw_arc.c
    lvgl/src/draw/sw/lv_draw_sw_border.c
    lvgl/src/draw/sw/lv_draw_sw_box_shadow.c
    lvgl/src/draw/sw/lv_draw_sw.c
    lvgl/src/draw/sw/lv_draw_sw_fill.c
    lvgl/src/draw/sw/lv_draw_sw_gradient.c
    lvgl/src/draw/sw/lv_draw_sw_img.c
    lvgl/src/draw/sw/lv_draw_sw_letter.c
    lvgl/src/draw/sw/lv_draw_sw_line.c
    lvgl/src/draw/sw/lv_draw_sw_mask.c
    lvgl/src/draw/sw/lv_draw_sw_mask_rect.c
    lvgl/src/draw/sw/lv_draw_sw_transform.c
    lvgl/src/draw/sw/lv_draw_sw_triangle.c
    lvgl/src/draw/sw/lv_draw_sw_vector.c
    lvgl/src/draw/sw/blend/lv_draw_sw_blend.c
    lvgl/src/draw/sw/blend/lv_draw_sw_blend_to_argb8888.c
    lvgl/src/draw/sw/blend/lv_draw_sw_blend_to_l8.c
    lvgl/src/draw/sw/blend/lv_draw_sw_blend_to_rgb888.c
    lvgl/src/draw/sw/blend/lv_draw_sw_blend_to_al88.c
    lvgl/src/draw/sw/blend/lv_draw_sw_blend_to_i1.c
    lvgl/src/draw/sw/blend/lv_draw_sw_blend_to_rgb565.c

    lvgl/src/font/lv_binfont_loader.c
    lvgl/src/font/lv_font.c
    lvgl/src/font/lv_font_dejavu_16_persian_hebrew.c
    lvgl/src/font/lv_font_fmt_txt.c
    lvgl/src/font/lv_font_montserrat_10.c
    lvgl/src/font/lv_font_montserrat_12.c
    lvgl/src/font/lv_font_montserrat_14.c
    lvgl/src/font/lv_font_montserrat_16.c
    lvgl/src/font/lv_font_montserrat_18.c
    lvgl/src/font/lv_font_montserrat_20.c
    lvgl/src/font/lv_font_montserrat_22.c
    lvgl/src/font/lv_font_montserrat_24.c
    lvgl/src/font/lv_font_montserrat_26.c
    lvgl/src/font/lv_font_montserrat_28.c
    lvgl/src/font/lv_font_montserrat_28_compressed.c
    lvgl/src/font/lv_font_montserrat_30.c
    lvgl/src/font/lv_font_montserrat_32.c
    lvgl/src/font/lv_font_montserrat_34.c
    lvgl/src/font/lv_font_montserrat_36.c
    lvgl/src/font/lv_font_montserrat_38.c
    lvgl/src/font/lv_font_montserrat_40.c
    lvgl/src/font/lv_font_montserrat_42.c
    lvgl/src/font/lv_font_montserrat_44.c
    lvgl/src/font/lv_font_montserrat_46.c
    lvgl/src/font/lv_font_montserrat_48.c
    lvgl/src/font/lv_font_montserrat_8.c
    lvgl/src/font/lv_font_simsun_14_cjk.c
    lvgl/src/font/lv_font_simsun_16_cjk.c
    lvgl/src/font/lv_font_unscii_16.c
    lvgl/src/font/lv_font_unscii_8.c

    lvgl/src/indev/lv_indev.c
    lvgl/src/indev/lv_indev_scroll.c

    lvgl/src/layouts/lv_layout.c
    lvgl/src/layouts/flex/lv_flex.c
    lvgl/src/layouts/grid/lv_grid.c

    lvgl/src/libs/bin_decoder/lv_bin_decoder.c

    lvgl/src/misc/lv_anim.c
    lvgl/src/misc/lv_anim_timeline.c
    lvgl/src/misc/lv_area.c
    lvgl/src/misc/lv_array.c
    lvgl/src/misc/lv_async.c
    lvgl/src/misc/lv_bidi.c
    lvgl/src/misc/lv_color.c
    lvgl/src/misc/lv_color_op.c
    lvgl/src/misc/lv_event.c
    lvgl/src/misc/lv_fs.c
    lvgl/src/misc/lv_ll.c
    lvgl/src/misc/lv_log.c
    lvgl/src/misc/lv_lru.c
    lvgl/src/misc/lv_math.c
    lvgl/src/misc/lv_matrix.c
    lvgl/src/misc/lv_palette.c
    lvgl/src/misc/lv_profiler_builtin.c
    lvgl/src/misc/lv_rb.c
    lvgl/src/misc/lv_style.c
    lvgl/src/misc/lv_style_gen.c
    lvgl/src/misc/lv_templ.c
    lvgl/src/misc/lv_text_ap.c
    lvgl/src/misc/lv_text.c
    lvgl/src/misc/lv_timer.c
    lvgl/src/misc/lv_utils.c
    lvgl/src/misc/cache/lv_cache.c
    lvgl/src/misc/cache/lv_cache_entry.c
    lvgl/src/misc/cache/lv_cache_lru_rb.c
    lvgl/src/misc/cache/lv_image_cache.c
    lvgl/src/misc/cache/lv_image_header_cache.c

    lvgl/src/osal/lv_freertos.c
    lvgl/src/osal/lv_os.c

    lvgl/src/others/sysmon/lv_sysmon.c
    lvgl/src/others/observer/lv_observer.c

    lvgl/src/stdlib/lv_mem.c
    lvgl/src/stdlib/clib/lv_mem_core_clib.c
    lvgl/src/stdlib/clib/lv_sprintf_clib.c
    lvgl/src/stdlib/clib/lv_string_clib.c

    lvgl/src/themes/lv_theme.c
    lvgl/src/themes/default/lv_theme_default.c
    lvgl/src/themes/mono/lv_theme_mono.c
    lvgl/src/themes/simple/lv_theme_simple.c

    lvgl/src/tick/lv_tick.c

    lvgl/src/widgets/animimage/lv_animimage.c
    lvgl/src/widgets/arc/lv_arc.c
    lvgl/src/widgets/bar/lv_bar.c
    lvgl/src/widgets/button/lv_button.c
    lvgl/src/widgets/buttonmatrix/lv_buttonmatrix.c
    lvgl/src/widgets/calendar/lv_calendar.c
    lvgl/src/widgets/calendar/lv_calendar_chinese.c
    lvgl/src/widgets/calendar/lv_calendar_header_arrow.c
    lvgl/src/widgets/calendar/lv_calendar_header_dropdown.c
    lvgl/src/widgets/canvas/lv_canvas.c
    lvgl/src/widgets/chart/lv_chart.c
    lvgl/src/widgets/checkbox/lv_checkbox.c
    lvgl/src/widgets/dropdown/lv_dropdown.c
    lvgl/src/widgets/image/lv_image.c
    lvgl/src/widgets/imagebutton/lv_imagebutton.c
    lvgl/src/widgets/keyboard/lv_keyboard.c
    lvgl/src/widgets/label/lv_label.c
    lvgl/src/widgets/line/lv_line.c
    lvgl/src/widgets/led/lv_led.c
    lvgl/src/widgets/list/lv_list.c
    lvgl/src/widgets/lottie/lv_lottie.c
    lvgl/src/widgets/menu/lv_menu.c
    lvgl/src/widgets/msgbox/lv_msgbox.c
    lvgl/src/widgets/objx_templ/lv_objx_templ.c
    lvgl/src/widgets/spinbox/lv_spinbox.c
    lvgl/src/widgets/property/lv_dropdown_properties.c
    lvgl/src/widgets/property/lv_image_properties.c
    lvgl/src/widgets/property/lv_keyboard_properties.c
    lvgl/src/widgets/property/lv_label_properties.c
    lvgl/src/widgets/property/lv_obj_properties.c
    lvgl/src/widgets/property/lv_roller_properties.c
    lvgl/src/widgets/property/lv_style_properties.c
    lvgl/src/widgets/property/lv_textarea_properties.c
    lvgl/src/widgets/roller/lv_roller.c
    lvgl/src/widgets/scale/lv_scale.c
    lvgl/src/widgets/slider/lv_slider.c
    lvgl/src/widgets/span/lv_span.c
    lvgl/src/widgets/spinbox/lv_spinbox.c
    lvgl/src/widgets/spinner/lv_spinner.c
    lvgl/src/widgets/switch/lv_switch.c
    lvgl/src/widgets/table/lv_table.c
    lvgl/src/widgets/tabview/lv_tabview.c
    lvgl/src/widgets/textarea/lv_textarea.c
    lvgl/src/widgets/tileview/lv_tileview.c
    lvgl/src/widgets/win/lv_win.c

    lvgl/demos/stress/lv_demo_stress.c

    lvgl/demos/benchmark/lv_demo_benchmark.c
    lvgl/demos/benchmark/assets/img_benchmark_avatar.c
    lvgl/demos/benchmark/assets/img_benchmark_cogwheel_alpha256.c
    lvgl/demos/benchmark/assets/img_benchmark_cogwheel_argb.c
    lvgl/demos/benchmark/assets/img_benchmark_cogwheel_indexed16.c
    lvgl/demos/benchmark/assets/img_benchmark_cogwheel_rgb.c
    lvgl/demos/benchmark/assets/lv_font_benchmark_montserrat_12_compr_az.c.c
    lvgl/demos/benchmark/assets/lv_font_benchmark_montserrat_16_compr_az.c.c
    lvgl/demos/benchmark/assets/lv_font_benchmark_montserrat_28_compr_az.c.c

    lvgl/demos/widgets/lv_demo_widgets.c
    lvgl/demos/widgets/assets/img_clothes.c
    lvgl/demos/widgets/assets/img_demo_widgets_avatar.c
    lvgl/demos/widgets/assets/img_demo_widgets_needle.c
    lvgl/demos/widgets/assets/img_lvgl_logo.c

    lvgl/examples/anim/lv_example_anim_1.c
    lvgl/examples/anim/lv_example_anim_2.c
    lvgl/examples/anim/lv_example_anim_3.c
    lvgl/examples/anim/lv_example_anim_timeline_1.c
)

ameba_list_append(private_includes
    ${CMAKE_CURRENT_SOURCE_DIR}/port/${c_SOC_TYPE}
)

ameba_list_append(private_definitions
    LV_CONF_INCLUDE_SIMPLE
)

ameba_list_append(private_compile_options
    -Wno-error
)

# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

###NOTE: For closed-source component, only build before release and libs are packaged into lib/application
ameba_add_internal_library(lvgl
    p_SOURCES
        ${private_sources}
    p_INCLUDES
        ${private_includes}
    p_DEFINITIONS
        ${private_definitions}
    p_COMPILE_OPTIONS
        ${private_compile_options}
)
##########################################################################################
