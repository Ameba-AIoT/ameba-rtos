set(DIR ${BASEDIR}/component/ui/LVGL/lv_drivers/amebadplus)
set(DIRLV ${BASEDIR}/component/ui/LVGL/lvgl)

set(
    MODULE_IFLAGS
    ${BASEDIR}/component/ui/LVGL/lv_drivers/interfaces
	${BASEDIR}/component/ui/drivers/st7789v/include
	${BASEDIR}/component/ui/LVGL/port/amebalite
	${DIRLV}/src
	${DIRLV}/src/core
	${DIRLV}/src/draw
	${DIRLV}/src/extra
	${DIRLV}/src/font
	${DIRLV}/src/hal
	${DIRLV}/src/misc
	${DIRLV}/src/widgets
)

set(MODULE_C_DEFINES __RTOS__)
set(MODULE_C_OPTIONS -Wno-incompatible-pointer-types)

set(
    CSRC
    ${DIR}/src/display.c
    ${DIR}/src/lv_hal.c
)


ameba_internal_library(lv_drivers)
target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_DEFINES})
target_compile_options(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_OPTIONS})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
