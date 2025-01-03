set(LIB_OPENTHREAD_PLATFORM_UTILS_DIR ${CMAKE_BINARY_DIR}/project_km4/asdk/make/wpan/openthread/openthread/examples/platforms/utils)
set(LIB_OPENTHREAD_CLI_DIR ${CMAKE_BINARY_DIR}/project_km4/asdk/make/wpan/openthread/openthread/src/cli)
set(LIB_OPENTHREAD_DIR ${CMAKE_BINARY_DIR}/project_km4/asdk/make/wpan/openthread/openthread/src/core)
set(LIB_OPENTHREAD_HDLC_DIR ${CMAKE_BINARY_DIR}/project_km4/asdk/make/wpan/openthread/openthread/src/lib/hdlc)
set(LIB_OPENTHREAD_PLATFORM_DIR ${CMAKE_BINARY_DIR}/project_km4/asdk/make/wpan/openthread/openthread/src/lib/platform)
set(LIB_OPENTHREAD_SPINEL_DIR ${CMAKE_BINARY_DIR}/project_km4/asdk/make/wpan/openthread/openthread/src/lib/spinel)
set(LIB_OPENTHREAD_NCP_DIR ${CMAKE_BINARY_DIR}/project_km4/asdk/make/wpan/openthread/openthread/src/ncp)
set(LIB_THIRD_TCPLP_DIR ${CMAKE_BINARY_DIR}/project_km4/asdk/make/wpan/openthread/openthread/third_party/tcplp)

# message ("LIB_OPENTHREAD_PLATFORM_UTILS_DIR : " ${LIB_OPENTHREAD_PLATFORM_UTILS_DIR})
# message ("LIB_OPENTHREAD_CLI_DIR : " ${LIB_OPENTHREAD_CLI_DIR})
# message ("LIB_OPENTHREAD_DIR : " ${LIB_OPENTHREAD_DIR})
# message ("LIB_OPENTHREAD_HDLC_DIR : " ${LIB_OPENTHREAD_HDLC_DIR})
# message ("LIB_OPENTHREAD_PLATFORM_DIR : " ${LIB_OPENTHREAD_PLATFORM_DIR})
# message ("LIB_OPENTHREAD_SPINEL_DIR : " ${LIB_OPENTHREAD_SPINEL_DIR})
# message ("LIB_OPENTHREAD_NCP_DIR : " ${LIB_OPENTHREAD_NCP_DIR})
# message ("LIB_THIRD_TCPLP_DIR : " ${LIB_THIRD_TCPLP_DIR})

list(
	APPEND LINK_THIRD_APP_LIB
	${LIB_OPENTHREAD_PLATFORM_UTILS_DIR}/libopenthread-platform-utils-static.a
	${LIB_OPENTHREAD_PLATFORM_DIR}/libopenthread-platform.a
)

if(CONFIG_802154_THREAD_BORDER_ROUTER_EN)
	list(
		APPEND LINK_THIRD_APP_LIB
		${LIB_OPENTHREAD_CLI_DIR}/libopenthread-cli-ftd.a
		${LIB_OPENTHREAD_DIR}/libopenthread-ftd.a
		${LIB_THIRD_TCPLP_DIR}/libtcplp-ftd.a
	)
    list(
        APPEND LINK_APP_LIB
        ${APP_LIB_DIR}/lib_otbr.a
    )
endif()

if(CONFIG_802154_THREAD_FTD_EN)
	list(
		APPEND LINK_THIRD_APP_LIB
		${LIB_OPENTHREAD_CLI_DIR}/libopenthread-cli-ftd.a
		${LIB_OPENTHREAD_DIR}/libopenthread-ftd.a
		${LIB_THIRD_TCPLP_DIR}/libtcplp-ftd.a
	)
endif()

if(CONFIG_802154_THREAD_MTD_EN)
	list(
		APPEND LINK_THIRD_APP_LIB
		${LIB_OPENTHREAD_CLI_DIR}/libopenthread-cli-mtd.a
		${LIB_OPENTHREAD_DIR}/libopenthread-mtd.a
		${LIB_THIRD_TCPLP_DIR}/libtcplp-mtd.a
	)
endif()

if(CONFIG_802154_THREAD_RCP_EN)
	list(
		APPEND LINK_THIRD_APP_LIB
		${LIB_OPENTHREAD_CLI_DIR}/libopenthread-cli-radio.a
		${LIB_OPENTHREAD_NCP_DIR}/libopenthread-rcp.a
		${LIB_OPENTHREAD_DIR}/libopenthread-radio-cli.a
		${LIB_OPENTHREAD_SPINEL_DIR}/libopenthread-spinel-rcp.a
		${LIB_OPENTHREAD_HDLC_DIR}/libopenthread-hdlc.a
	)
endif()

if(CONFIG_802154_THREAD_RADIO_RCP_EN)
	list(
		APPEND LINK_THIRD_APP_LIB
		${LIB_OPENTHREAD_HDLC_DIR}/libopenthread-hdlc.a
		${LIB_OPENTHREAD_SPINEL_DIR}/libopenthread-radio-spinel.a
		${LIB_OPENTHREAD_SPINEL_DIR}/libopenthread-spinel-rcp.a
	)
else()
	list(
		APPEND LINK_APP_LIB
		${APP_LIB_DIR}/lib_wpan_sw_radio.a
	)
endif()