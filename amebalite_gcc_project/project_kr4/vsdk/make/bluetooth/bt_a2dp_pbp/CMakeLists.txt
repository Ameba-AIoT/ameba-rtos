set(BT_DIR ${BASEDIR}/component/bluetooth)
set(AUDIO_HAL_DIR ${BASEDIR}/component/audio)
set(BT_AUDIO_DIR ${BT_DIR}/bt_audio)
set(BT_CODEC_SBC ${BT_AUDIO_DIR}/bt_codec/sbc)
set(A2DP_PBP_DIR ${BT_DIR}/example/bt_a2dp_pbp)
set(BLE_AUDIO_COMMON_DIR ${BT_DIR}/example/ble_audio_common)

set(
    MODULE_IFLAGS
	${BT_AUDIO_DIR}/include
	${BT_AUDIO_DIR}/bt_codec
	${BT_CODEC_SBC}/decoder/include
	${BT_CODEC_SBC}/encoder/include
	${BLE_AUDIO_COMMON_DIR}
	${AUDIO_HAL_DIR}/interfaces
	${AUDIO_HAL_DIR}/base/osal/osal_c/interfaces
)

set(
    CSRC
    ${A2DP_PBP_DIR}/a2dp_pbp.c
)

target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
