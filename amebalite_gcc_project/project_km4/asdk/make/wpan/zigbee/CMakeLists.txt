set(ZB_DIR ${BASEDIR}/component/wpan/zigbee)
set(ZBOSS_LIB_DIR ${BASEDIR}/component/wpan/zigbee/lib)
set(SW_RADIO ${BASEDIR}/component/wpan/platform/sw_radio)
set(ZB_RTK_SUPPORT_DIR ${BASEDIR}/component/wpan/zigbee/rtk_zb_support)
set(ZB_STACK_DIR ${BASEDIR}/component/wpan/zigbee/zboss_stable)

##########remove warning#########################
set(CMAKE_WARN_DEPRECATED OFF)
set(CMAKE_WARN_ERROR ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
#################################################

set(
    MODULE_C_DEFINES
	ZB_ERROR_PRINT_TO_LOG=1
	CONFIG_ZIGBEE_ERROR_TO_STRING_ENABLED=1
	LIBZBOSS_CONFIG_FILE=\"libzboss_config.h\"
)

set(
    MODULE_IFLAGS
	${BASEDIR}/component/soc/amebalite/fwlib/include/
    ${BASEDIR}/component/component/os/os_dep/
	${ZB_DIR}/include/
	${ZB_RTK_SUPPORT_DIR}/config/
	${ZB_RTK_SUPPORT_DIR}/include/

	${ZB_STACK_DIR}/include/	
	${ZB_STACK_DIR}/include/ha/	
	${ZB_STACK_DIR}/include/se/	
	${ZB_STACK_DIR}/include/zcl/	
	${ZB_STACK_DIR}/include/zgp/	
	${ZB_STACK_DIR}/include/zgpd/	
	${ZB_STACK_DIR}/include/zll/
)


if(CONFIG_802154_ZIGBEE_ROLE_ZED)
list(
	APPEND MODULE_C_DEFINES
	ZB_ED_ROLE=1
)
endif()

add_subdirectory(api)
add_subdirectory(zb_support)
add_subdirectory(example)
add_subdirectory(zboss)