set(BT_DIR ${BASEDIR}/component/bluetooth)

set(CSRC ${BT_DIR}/example/ble_mesh_device/mesh_device.c)

target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
