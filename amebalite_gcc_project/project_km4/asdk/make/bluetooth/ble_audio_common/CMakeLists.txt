set(BT_DIR ${BASEDIR}/component/bluetooth)

set(
    CSRC
    ${BT_DIR}/example/ble_audio_common/app_bt_le_audio_common.c
)

target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
