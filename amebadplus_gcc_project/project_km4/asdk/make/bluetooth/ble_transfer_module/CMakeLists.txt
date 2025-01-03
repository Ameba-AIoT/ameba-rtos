set(BT_DIR ${BASEDIR}/component/bluetooth)

set(MODULE_IFLAGS ${BT_DIR}/example/ble_transfer_module)

set(
    CSRC 
    ${BT_DIR}/example/ble_transfer_module/transfer_module_main.c 
	${BT_DIR}/example/ble_transfer_module/transfer_module_server.c 
	${BT_DIR}/example/ble_transfer_module/transfer_module_client.c 
	${BT_DIR}/example/bt_atcmd/atcmd_bt_transfer_module.c 
	${BT_DIR}/example/gatt_service/client/rtk_gcs_client.c
)

target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
