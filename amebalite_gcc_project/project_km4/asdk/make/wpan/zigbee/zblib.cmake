list(
	APPEND LINK_APP_LIB
	${APP_LIB_DIR}/lib_wpan_sw_radio.a
)

if(CONFIG_802154_ZIGBEE_MAC_TEST_EN)
list(
	APPEND LINK_APP_LIB
	${APP_LIB_DIR}/lib_zboss_mac_test.a
)
endif()

if(CONFIG_802154_ZIGBEE_ZCP_TEST_EN)
if(CONFIG_802154_ZIGBEE_ZCP_DEFAULT_EN)
if(CONFIG_802154_ZIGBEE_ROLE_ZC)
list(
	APPEND LINK_APP_LIB
	${APP_LIB_DIR}/lib_zboss_zcp_test_default_zc_zr.a
)
endif()

if(CONFIG_802154_ZIGBEE_ROLE_ZR)
list(
	APPEND LINK_APP_LIB
	${APP_LIB_DIR}/lib_zboss_zcp_test_default_zc_zr.a
)
endif()

if(CONFIG_802154_ZIGBEE_ROLE_ZED)
list(
	APPEND LINK_APP_LIB
	${APP_LIB_DIR}/lib_zboss_zcp_test_default_ed.a
)
endif()
endif()

if(CONFIG_802154_ZIGBEE_ZCP_GPPB_EN)
if(CONFIG_802154_ZIGBEE_ROLE_ZC)
list(
	APPEND LINK_APP_LIB
	${APP_LIB_DIR}/lib_zboss_zcp_test_gppb_zc.a
)
endif()

if(CONFIG_802154_ZIGBEE_ROLE_ZR)
list(
	APPEND LINK_APP_LIB
	${APP_LIB_DIR}/lib_zboss_zcp_test_gppb_zr.a
)
endif()
endif()
endif()

if(CONFIG_802154_ZIGBEE_REGRESSION_TEST_EN)

endif()

if(CONFIG_802154_ZIGBEE_APPLICATION_EN)
if(CONFIG_802154_ZIGBEE_ROLE_ZC)
list(
	APPEND LINK_APP_LIB
	${APP_LIB_DIR}/lib_zboss_zc_zr.a
)
endif()

if(CONFIG_802154_ZIGBEE_ROLE_ZR)
list(
	APPEND LINK_APP_LIB
	${APP_LIB_DIR}/lib_zboss_zc_zr.a
)
endif()

if(CONFIG_802154_ZIGBEE_ROLE_ZED)
list(
	APPEND LINK_APP_LIB
	${APP_LIB_DIR}/lib_zboss_ed.a
)
endif()
endif()