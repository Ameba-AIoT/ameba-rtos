set(BT_DIR ${BASEDIR}/component/bluetooth)

set(CSRC ${BT_DIR}/example/ble_mesh_device/mesh_device.c)

target_sources(target_img2_${PROJECT_NAME} PRIVATE ${CSRC})