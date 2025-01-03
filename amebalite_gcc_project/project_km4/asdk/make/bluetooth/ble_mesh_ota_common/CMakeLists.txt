set(BT_DIR ${BASEDIR}/component/bluetooth)

set(
	CSRC
	${BT_DIR}/example/ble_mesh_ota_common/rtk_bt_device_firmware_update_amebalite.c
	${BT_DIR}/example/ble_mesh_ota_common/rtk_bt_device_firmware_update_interface.c
)

target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
