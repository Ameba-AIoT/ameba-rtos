set(BT_DIR ${BASEDIR}/component/bluetooth)
set(BT_APP_AUDIO_DATA ${BT_DIR}/example/bt_app_audio_data)
set(BT_AUDIO_DIR ${BT_DIR}/bt_audio)
set(HFP_DIR ${BT_DIR}/example/bt_hfp)

set(
    MODULE_IFLAGS
    ${HFP_DIR}
	${BT_APP_AUDIO_DATA}
	${BT_AUDIO_DIR}/include
	${BT_AUDIO_DIR}/bt_codec
)

set(CSRC ${HFP_DIR}/hfp.c)

target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
