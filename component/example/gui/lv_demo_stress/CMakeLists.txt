ameba_internal_library(example_lv_demo_stress)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_lv_demo_stress.c
)


target_include_directories(
    ${CURRENT_LIB_NAME} PRIVATE

    ${BASEDIR}/component/ui/LVGL/lvgl
    ${BASEDIR}/component/ui/LVGL/lvgl/demos/stress
    ${BASEDIR}/component/ui/LVGL/lv_drivers/interfaces
    ${BASEDIR}/component/ui/LVGL/port/${CMAKE_PROJECT_NAME}
)
