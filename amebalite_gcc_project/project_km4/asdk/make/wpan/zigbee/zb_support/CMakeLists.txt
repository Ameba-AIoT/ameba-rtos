set(TIMER ${BASEDIR}/component/wpan/platform/timer)
set(FLASH  ${BASEDIR}/component/wpan/platform/flash)
set(PLATFORM_CONFIG ${BASEDIR}/component/wpan/platform/config)

list(
	APPEND MODULE_IFLAGS
	${TIMER}/
    ${FLASH}/
	${PLATFORM_CONFIG}/
)

set(SW_RADIO ${BASEDIR}/component/wpan/platform/sw_radio)
list(
	APPEND MODULE_IFLAGS
	${SW_RADIO}/phy/common/radio_algorithm/include/
	${SW_RADIO}/phy/common/radio_api/
	${SW_RADIO}/mac/
	${SW_RADIO}/mac/config
	${SW_RADIO}/mac/mac_features/
	${SW_RADIO}/mac/mac_features/ack_generator/
	${SW_RADIO}/mac/rsch/
)

set(CSRC)

list(
    APPEND CSRC 
	${ZB_RTK_SUPPORT_DIR}/zb_osif_entry.c
	${ZB_RTK_SUPPORT_DIR}/zb_osif_common.c
	${ZB_RTK_SUPPORT_DIR}/zb_osif_logger.c
	${ZB_RTK_SUPPORT_DIR}/zb_osif_nvram.c
	${ZB_RTK_SUPPORT_DIR}/zb_osif_transceiver.c
	${ZB_RTK_SUPPORT_DIR}/zb_osif_transceiver_mem.c
	${ZB_RTK_SUPPORT_DIR}/zb_osif_timer.c
)

ameba_internal_library(zb_support)
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_DEFINES})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})