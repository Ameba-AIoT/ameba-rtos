ameba_internal_library(example_lv_demo_widgets)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_lv_demo_widgets.c
)


target_include_directories(
    ${CURRENT_LIB_NAME} PRIVATE

    ${BASEDIR}/component/ui/LVGL/lvgl
    ${BASEDIR}/component/ui/LVGL/lvgl/demos/widgets
    ${BASEDIR}/component/ui/LVGL/lv_drivers/interfaces
    ${BASEDIR}/component/ui/LVGL/port/${CMAKE_PROJECT_NAME}
)
