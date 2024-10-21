set(BT_DIR ${BASEDIR}/component/bluetooth)
set(BT_APP_AUDIO_DATA ${BT_DIR}/example/bt_app_audio_data)
set(BT_AUDIO_DIR ${BT_DIR}/bt_audio)
set(A2DP_DIR ${BT_DIR}/example/bt_a2dp)
set(BT_CODEC_SBC ${BT_AUDIO_DIR}/bt_codec/sbc)

set(
    MODULE_IFLAGS
	${A2DP_DIR}
	${BT_APP_AUDIO_DATA}
	${BT_AUDIO_DIR}/include
	${BT_AUDIO_DIR}/bt_codec
	${BT_CODEC_SBC}/decoder/include
	${BT_CODEC_SBC}/encoder/include
)

set(
    CSRC
    ${A2DP_DIR}/a2dp.c
)

target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
