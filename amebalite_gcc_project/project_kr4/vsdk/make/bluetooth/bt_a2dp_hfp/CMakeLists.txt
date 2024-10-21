set(BT_DIR ${BASEDIR}/component/bluetooth)
set(BT_APP_AUDIO_DATA ${BT_DIR}/example/bt_app_audio_data)
set(BT_AUDIO_DIR ${BT_DIR}/bt_audio)
set(BT_AUDIO_A2DP_HFP_DIR ${BT_DIR}/example/bt_a2dp_hfp)
set(BT_CODEC_SBC ${BT_AUDIO_DIR}/bt_codec/sbc)

set(
    MODULE_IFLAGS
	${BT_AUDIO_A2DP_HFP_DIR}
	${BT_APP_AUDIO_DATA}
	${BT_AUDIO_DIR}/include
	${BT_AUDIO_DIR}/bt_codec
	${BT_CODEC_SBC}/decoder/include
	${BT_CODEC_SBC}/encoder/include
)

set(
    CSRC
    ${BT_AUDIO_A2DP_HFP_DIR}/a2dp_hfp.c
)

target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
