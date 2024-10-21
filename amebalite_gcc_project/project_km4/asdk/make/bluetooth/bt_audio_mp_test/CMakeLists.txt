set(BT_DIR ${BASEDIR}/component/bluetooth)
set(BT_AUDIO_DIR ${BT_DIR}/bt_audio)
set(BT_APP_AUDIO_DATA ${BT_DIR}/example/bt_app_audio_data)
set(BT_AUDIO_MP_TEST_DIR ${BT_DIR}/example/bt_audio_mp_test)
set(BT_CODEC_SBC ${BT_AUDIO_DIR}/bt_codec/sbc)

set(
    MODULE_IFLAGS
	${BT_AUDIO_MP_TEST_DIR}
	${BT_APP_AUDIO_DATA}
	${BT_AUDIO_DIR}/include
	${BT_AUDIO_DIR}/bt_codec
	${BT_CODEC_SBC}/decoder/include
	${BT_CODEC_SBC}/encoder/include
)

set(
    CSRC
	${BT_AUDIO_MP_TEST_DIR}/bt_audio_mp_test.c
)

target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
