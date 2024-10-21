set(BT_DIR ${BASEDIR}/component/bluetooth)
set(BT_APP_AUDIO_DATA ${BT_DIR}/example/bt_app_audio_data)

set(
    MODULE_IFLAGS
	${BT_APP_AUDIO_DATA}
	${BT_DIR}/example/ble_audio_common
)

set(
    CSRC
	${BT_APP_AUDIO_DATA}/app_audio_data.c
)

target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
