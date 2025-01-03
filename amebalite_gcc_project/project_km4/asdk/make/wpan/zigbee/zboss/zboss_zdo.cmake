list(
	APPEND CSRC 
	${ZB_STACK_DIR}/zdo/af_descriptor.c
	${ZB_STACK_DIR}/zdo/af_interpan.c
	${ZB_STACK_DIR}/zdo/af_rx.c
	${ZB_STACK_DIR}/zdo/joining_list_mgmt.c
	${ZB_STACK_DIR}/zdo/ncp_weak.c
	${ZB_STACK_DIR}/zdo/test_profile.c
	${ZB_STACK_DIR}/zdo/zb_zboss_api_common.c
	${ZB_STACK_DIR}/zdo/zb_zboss_api_default.c
	${ZB_STACK_DIR}/zdo/zdo_app.c
	${ZB_STACK_DIR}/zdo/zdo_app_common.c
	${ZB_STACK_DIR}/zdo/zdo_app_join.c
	${ZB_STACK_DIR}/zdo/zdo_app_leave.c
	${ZB_STACK_DIR}/zdo/zdo_app_prod_conf.c
	${ZB_STACK_DIR}/zdo/zdo_bind_manage.c
	${ZB_STACK_DIR}/zdo/zdo_channel_manager.c
	${ZB_STACK_DIR}/zdo/zdo_commissioning.c
	${ZB_STACK_DIR}/zdo/zdo_common.c
	${ZB_STACK_DIR}/zdo/zdo_diagnostics.c
	${ZB_STACK_DIR}/zdo/zdo_disc_cli.c
	${ZB_STACK_DIR}/zdo/zdo_disc_srv.c
	${ZB_STACK_DIR}/zdo/zdo_ed_aging.c
	${ZB_STACK_DIR}/zdo/zdo_formation.c
	${ZB_STACK_DIR}/zdo/zdo_hubs.c
	${ZB_STACK_DIR}/zdo/zdo_nwk_manage_cli.c
	${ZB_STACK_DIR}/zdo/zdo_nwk_manage_srv.c
	${ZB_STACK_DIR}/zdo/zdo_poll.c
	${ZB_STACK_DIR}/zdo/zdo_promisc.c
	${ZB_STACK_DIR}/zdo/zdo_resp_validate.c
	${ZB_STACK_DIR}/zdo/zdo_rx.c
	${ZB_STACK_DIR}/zdo/zdo_wwah_parent_classification.c
	${ZB_STACK_DIR}/zdo/zdo_wwah_survey_beacons.c
	
	# ${ZB_STACK_DIR}/zdo/zdo_cb_stubs.c
)