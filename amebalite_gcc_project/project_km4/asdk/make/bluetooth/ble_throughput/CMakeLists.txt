set(BT_DIR ${BASEDIR}/component/bluetooth)

set(
    CSRC 
	${BT_DIR}/example/ble_throughput/throughput_main.c 
	${BT_DIR}/example/ble_throughput/throughput_server.c 
	${BT_DIR}/example/ble_throughput/throughput_client.c 
	${BT_DIR}/example/ble_throughput/throughput_common.c 
	${BT_DIR}/example/bt_atcmd/atcmd_bt_throughput_test.c 
	${BT_DIR}/example/gatt_service/client/rtk_gcs_client.c
)

set(MODULE_IFLAGS ${BT_DIR}/example/ble_throughput)


target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
