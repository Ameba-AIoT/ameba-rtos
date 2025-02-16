set(BT_DIR ${BASEDIR}/component/bluetooth)
set(BLE_AUDIO_COMMON_DIR ${BT_DIR}/example/ble_audio_common)
set(BT_APP_AUDIO_DATA ${BT_DIR}/example/bt_app_audio_data)

set(
    MODULE_IFLAGS
    ${BLE_AUDIO_COMMON_DIR}
	${BT_APP_AUDIO_DATA}
)

set(
    CSRC
    ${BT_DIR}/example/ble_audio/cap.c
    ${BT_DIR}/example/ble_audio/pbp.c
    ${BT_DIR}/example/ble_audio/tmap.c
)

target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
