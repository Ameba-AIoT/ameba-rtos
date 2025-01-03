set(DIR ${BASEDIR}/component/bluetooth/example)

set(MODULE_IFLAGS ${DIR}/ble_audio_common)

set(
    CSRC
	${DIR}/bt_atcmd/bt_utils.c
	${DIR}/bt_atcmd/atcmd_bt_common.c
	${DIR}/bt_atcmd/atcmd_bt_example.c
	${DIR}/bt_atcmd/atcmd_bt_gap.c
	${DIR}/bt_atcmd/atcmd_bt_le_gap.c
	${DIR}/bt_atcmd/atcmd_bt_br_gap.c
	${DIR}/bt_atcmd/atcmd_bt_gatts.c
	${DIR}/bt_atcmd/atcmd_bt_gattc.c
	${DIR}/bt_atcmd/atcmd_bt_vendor.c
	${DIR}/bt_atcmd/atcmd_bt_sdp.c
	${DIR}/bt_atcmd/atcmd_bt_a2dp.c
	${DIR}/bt_atcmd/atcmd_bt_avrcp.c
	${DIR}/bt_atcmd/atcmd_bt_spp.c
	${DIR}/bt_atcmd/atcmd_bt_rfc.c
	${DIR}/bt_atcmd/atcmd_bt_hid.c
	${DIR}/bt_atcmd/atcmd_bt_hfp.c
	${DIR}/bt_atcmd/atcmd_bt_pbap.c
	${DIR}/bt_atcmd/atcmd_bt_bap.c
	${DIR}/bt_atcmd/atcmd_bt_cap.c
	${DIR}/bt_atcmd/atcmd_bt_pbp.c
	${DIR}/bt_atcmd/atcmd_bt_tmap.c
	${DIR}/bt_atcmd/atcmd_bt_gmap.c
	${DIR}/bt_atcmd/atcmd_bt_mesh_common.c
	${DIR}/bt_atcmd/atcmd_bt_mesh_config_model.c
	${DIR}/bt_atcmd/atcmd_bt_mesh_datatrans_model.c
	${DIR}/bt_atcmd/atcmd_bt_mesh_device_firmware_update_model.c
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
	${DIR}/bt_atcmd/atcmd_bt_mesh_subnet_bridge_model.c
	${DIR}/bt_atcmd/atcmd_bt_mesh_private_beacon_model.c
)

target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
