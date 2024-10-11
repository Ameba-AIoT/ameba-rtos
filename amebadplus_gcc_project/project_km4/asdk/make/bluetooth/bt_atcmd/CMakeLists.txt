set(DIR ${BASEDIR}/component/bluetooth/example)

set(
    CSRC
    ${DIR}/bt_atcmd/bt_utils.c
	${DIR}/bt_atcmd/atcmd_bt_common.c
	${DIR}/bt_atcmd/atcmd_bt_device.c
	${DIR}/bt_atcmd/atcmd_bt_example.c
	${DIR}/bt_atcmd/atcmd_bt_gap.c
	${DIR}/bt_atcmd/atcmd_bt_le_gap.c
	${DIR}/bt_atcmd/atcmd_bt_gatts.c
	${DIR}/bt_atcmd/atcmd_bt_gattc.c
	${DIR}/bt_atcmd/atcmd_bt_vendor.c
	${DIR}/bt_atcmd/atcmd_bt_mesh_common.c
	${DIR}/bt_atcmd/atcmd_bt_mesh_config_model.c
	${DIR}/bt_atcmd/atcmd_bt_mesh_datatrans_model.c
	${DIR}/bt_atcmd/atcmd_bt_mesh_goo_model.c
	${DIR}/bt_atcmd/atcmd_bt_mesh_remote_prov_model.c
	${DIR}/bt_atcmd/atcmd_bt_mesh_light_model.c
	${DIR}/bt_atcmd/atcmd_bt_mesh_time_model.c
	${DIR}/bt_atcmd/atcmd_bt_mesh_scheduler_model.c
	${DIR}/bt_atcmd/atcmd_bt_mesh_scene_model.c
	${DIR}/bt_atcmd/atcmd_bt_mesh_generic_default_transition_time.c
	${DIR}/bt_atcmd/atcmd_bt_mesh_generic_model.c
	${DIR}/bt_atcmd/atcmd_bt_mesh_sensor_model.c
	${DIR}/bt_atcmd/atcmd_bt_mesh_health_model.c
	${DIR}/bt_atcmd/atcmd_bt_mesh_directed_forwarding.c
)

target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
