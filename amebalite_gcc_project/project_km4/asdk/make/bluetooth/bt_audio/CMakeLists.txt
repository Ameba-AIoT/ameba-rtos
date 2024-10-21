set(BT_DIR ${BASEDIR}/component/bluetooth)
set(BT_AUDIO_DIR ${BT_DIR}/bt_audio)
set(AUDIO_HAL_DIR ${BASEDIR}/component/audio)
set(BT_CODEC_SBC ${BT_AUDIO_DIR}/bt_codec/sbc)
set(BT_CODEC_LC3 ${BT_AUDIO_DIR}/bt_codec/lc3)

set(
    MODULE_IFLAGS
	${BT_DIR}/rtk_stack/example/ble_audio
	${BT_DIR}/rtk_stack/inc/bluetooth/leaudio
	${BT_AUDIO_DIR}/include
	${BT_AUDIO_DIR}/bt_codec
	${BT_AUDIO_DIR}/audio_sync
	${AUDIO_HAL_DIR}/interfaces
	${AUDIO_HAL_DIR}/base/osal/osal_c/interfaces
	${BT_CODEC_SBC}/decoder/include
	${BT_CODEC_SBC}/encoder/include
	${BT_CODEC_LC3}/include
	${BT_CODEC_LC3}/src
)

set(
    CSRC
	${BT_AUDIO_DIR}/bt_audio_intf.c
	${BT_AUDIO_DIR}/bt_audio_resample.c
	${BT_AUDIO_DIR}/bt_audio_track_api.c
	${BT_AUDIO_DIR}/bt_audio_record_api.c
	${BT_AUDIO_DIR}/bt_audio_codec_wrapper.c
	${BT_AUDIO_DIR}/bt_codec/sbc_codec_entity.c
	${BT_AUDIO_DIR}/bt_codec/lc3_codec_entity.c
	${BT_AUDIO_DIR}/bt_codec/cvsd_codec_entity.c
)

target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
