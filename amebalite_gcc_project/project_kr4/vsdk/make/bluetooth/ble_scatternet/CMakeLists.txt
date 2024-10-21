set(BT_DIR ${BASEDIR}/component/bluetooth)

set(
    CSRC
	${BT_DIR}/example/ble_scatternet/scatternet.c
	${BT_DIR}/example/gatt_service/server/rtk_bas.c
	${BT_DIR}/example/gatt_service/server/rtk_simple_ble_service.c
	${BT_DIR}/example/gatt_service/server/rtk_hrs.c
	${BT_DIR}/example/gatt_service/server/rtk_dis.c
	${BT_DIR}/example/gatt_service/server/rtk_ias.c
	${BT_DIR}/example/gatt_service/server/rtk_hids_kb.c
	${BT_DIR}/example/gatt_service/server/rtk_gls.c
	${BT_DIR}/example/gatt_service/server/rtk_long_uuid_service.c
	${BT_DIR}/example/gatt_service/client/rtk_gcs_client.c
	${BT_DIR}/example/gatt_service/client/rtk_bas_client.c
	${BT_DIR}/example/gatt_service/client/rtk_gaps_client.c
	${BT_DIR}/example/gatt_service/client/rtk_simple_ble_client.c
	${BT_DIR}/example/gatt_service/server/rtk_cte_service.c
	${BT_DIR}/example/gatt_service/client/rtk_cte_client.c
)

target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
