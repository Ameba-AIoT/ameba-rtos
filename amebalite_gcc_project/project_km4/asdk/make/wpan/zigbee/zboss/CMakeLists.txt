list(
	APPEND MODULE_IFLAGS
    ${ZB_EXAMPLE_DIR}/
	${ZB_STACK_DIR}/include/	
	${ZB_STACK_DIR}/include/ha/	
	${ZB_STACK_DIR}/include/se/	
	${ZB_STACK_DIR}/include/zcl/	
	${ZB_STACK_DIR}/include/zgp/	
	${ZB_STACK_DIR}/include/zgpd/	
	${ZB_STACK_DIR}/include/zll/		
)

if(CONFIG_802154_ZIGBEE_APPLICATION_EN)
add_subdirectory(application)
endif()
if(CONFIG_802154_ZIGBEE_MAC_TEST_EN)
add_subdirectory(mac_test)
endif()
if(CONFIG_802154_ZIGBEE_REGRESSION_TEST_EN)
add_subdirectory(regression_test)
endif()
if(CONFIG_802154_ZIGBEE_ZCP_TEST_EN)
add_subdirectory(zcp_test)
endif()