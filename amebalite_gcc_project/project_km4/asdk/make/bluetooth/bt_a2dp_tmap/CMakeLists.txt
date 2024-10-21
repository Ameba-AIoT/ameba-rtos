set(BT_DIR ${BASEDIR}/component/bluetooth)
set(BT_APP_AUDIO_DATA ${BT_DIR}/example/bt_app_audio_data)
set(BT_AUDIO_DIR ${BT_DIR}/bt_audio)
set(BT_CODEC_SBC ${BT_AUDIO_DIR}/bt_codec/sbc)
set(A2DP_TMAP_DIR ${BT_DIR}/example/bt_a2dp_tmap)
set(BLE_AUDIO_COMMON_DIR ${BT_DIR}/example/ble_audio_common)

set(
    MODULE_IFLAGS
	${BLE_AUDIO_COMMON_DIR}
	${BT_APP_AUDIO_DATA}
	${BT_AUDIO_DIR}/include
	${BT_AUDIO_DIR}/bt_codec
	${BT_CODEC_SBC}/decoder/include
	${BT_CODEC_SBC}/encoder/include
)

set(
    CSRC
	${A2DP_TMAP_DIR}/a2dp_tmap.c
	${A2DP_TMAP_DIR}/tmap_a2dp_source.c
)

target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
