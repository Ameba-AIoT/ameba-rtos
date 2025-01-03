set(SW_RADIO ${BASEDIR}/component/wpan/platform/sw_radio)

list(
    APPEND MODULE_IFLAGS
    ${SW_RADIO}/phy/common/radio_api/
    ${SW_RADIO}/mac/
    ${SW_RADIO}/mac/mac_features/
    ${SW_RADIO}/mac/mac_features/ack_generator/
    ${SW_RADIO}/mac/rsch/
    ${SW_RADIO}/mac/rsch/coex/
    ${SW_RADIO}/mac/rsch/raal/
    ${SW_RADIO}/mac/rsch/raal/softdevice/
)

list(
	APPEND MODULE_C_DEFINES
	RTK_802154_HOST_IPC=1
)

set(CSRC)

list(
	APPEND CSRC 
	${SW_RADIO}/phy/common/radio_api/rtk_802154_radio.c
	${SW_RADIO}/mac/mac_features/ack_generator/rtk_802154_ack_data.c
	${SW_RADIO}/mac/mac_features/ack_generator/rtk_802154_ack_generator.c
	${SW_RADIO}/mac/mac_features/ack_generator/rtk_802154_enh_ack_generator.c
	${SW_RADIO}/mac/mac_features/ack_generator/rtk_802154_imm_ack_generator.c
	${SW_RADIO}/mac/mac_features/rtk_802154_csma_ca.c
	${SW_RADIO}/mac/mac_features/rtk_802154_delayed_trx.c
	${SW_RADIO}/mac/mac_features/rtk_802154_filter.c
	${SW_RADIO}/mac/mac_features/rtk_802154_frame_parser.c
	${SW_RADIO}/mac/mac_features/rtk_802154_precise_ack_timeout.c
	${SW_RADIO}/mac/mac_features/rtk_802154_ifs.c
	${SW_RADIO}/mac/mac_features/rtk_802154_ie_writer.c
	${SW_RADIO}/mac/mac_features/rtk_802154_security_pib.c
	${SW_RADIO}/mac/mac_features/rtk_802154_security_writer.c
	${SW_RADIO}/mac/rtk_802154_aes_ccm.c
	${SW_RADIO}/mac/rtk_802154_encrypt.c
	${SW_RADIO}/mac/rtk_802154_core_hooks.c
	${SW_RADIO}/mac/rtk_802154_core.c
	${SW_RADIO}/mac/rtk_802154_critical_section.c
	${SW_RADIO}/mac/rtk_802154_pib.c
	${SW_RADIO}/mac/rtk_802154_rx_buffer.c
	${SW_RADIO}/mac/rtk_802154.c
	${SW_RADIO}/mac/rtk_802154_debug.c
	${SW_RADIO}/mac/rtk_802154_notification.c
)

ameba_app_library(wpan_sw_radio)
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_DEFINES})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})