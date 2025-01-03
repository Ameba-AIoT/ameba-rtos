set(ZB_EXAMPLE_DIR ${BASEDIR}/component/wpan/zigbee/example/ha_on_off_light_n_switch/on_off_light_switch)
set(ZB_EXAMPLE_COMMON_DIR ${BASEDIR}/component/wpan/zigbee/example/common)

list(
	APPEND MODULE_IFLAGS
    ${ZB_EXAMPLE_DIR}/
	${ZB_EXAMPLE_COMMON_DIR}/light_driver/
	${ZB_EXAMPLE_COMMON_DIR}/switch_driver/
)

set(CSRC)

list(
	APPEND CSRC 
	${ZB_EXAMPLE_DIR}/example.c
	${ZB_EXAMPLE_COMMON_DIR}/light_driver/light_driver.c
	${ZB_EXAMPLE_COMMON_DIR}/switch_driver/switch_driver.c
)

ameba_internal_library(zb_on_off_light_switch)
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_DEFINES})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})