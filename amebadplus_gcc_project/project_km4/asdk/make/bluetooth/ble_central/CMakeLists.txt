set(BT_DIR ${BASEDIR}/component/bluetooth)

set(
    CSRC 
    ${BT_DIR}/example/ble_central/central.c 
	${BT_DIR}/example/gatt_service/client/rtk_gcs_client.c 
	${BT_DIR}/example/gatt_service/client/rtk_bas_client.c 
	${BT_DIR}/example/gatt_service/client/rtk_gaps_client.c 
	${BT_DIR}/example/gatt_service/client/rtk_simple_ble_client.c
)

target_sources(target_img2_${PROJECT_NAME} PRIVATE ${CSRC})