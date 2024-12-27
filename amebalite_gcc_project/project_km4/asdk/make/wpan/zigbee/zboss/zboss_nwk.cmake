list(
	APPEND CSRC 
	${ZB_STACK_DIR}/nwk/nwk_address_assign.c
	${ZB_STACK_DIR}/nwk/nwk_address_conflict.c
	${ZB_STACK_DIR}/nwk/nwk_broadcasting.c
	${ZB_STACK_DIR}/nwk/nwk_cr_formation.c
	${ZB_STACK_DIR}/nwk/nwk_cr_join.c
	${ZB_STACK_DIR}/nwk/nwk_cr_mesh_routing.c
	${ZB_STACK_DIR}/nwk/nwk_cr_parent.c
	${ZB_STACK_DIR}/nwk/nwk_cr_permit_join.c
	${ZB_STACK_DIR}/nwk/nwk_cr_route_discovery.c
	${ZB_STACK_DIR}/nwk/nwk_cr_tree_routing.c
	${ZB_STACK_DIR}/nwk/nwk_discovery.c
	${ZB_STACK_DIR}/nwk/nwk_formation.c
	${ZB_STACK_DIR}/nwk/nwk_join.c
	${ZB_STACK_DIR}/nwk/nwk_link_power_delta.c
	${ZB_STACK_DIR}/nwk/nwk_link_status.c
	${ZB_STACK_DIR}/nwk/nwk_main.c
	${ZB_STACK_DIR}/nwk/nwk_mm.c
	${ZB_STACK_DIR}/nwk/nwk_multimac_proxy.c
	${ZB_STACK_DIR}/nwk/nwk_neighbor.c
	${ZB_STACK_DIR}/nwk/nwk_neighbor_utility.c
	${ZB_STACK_DIR}/nwk/nwk_nlme.c
	${ZB_STACK_DIR}/nwk/nwk_panid_conflict.c
	${ZB_STACK_DIR}/nwk/nwk_srouting.c
	${ZB_STACK_DIR}/nwk/zb_nwk_ed_aging.c
	
	# ${ZB_STACK_DIR}/nwk/nwk_cb_stubs.c
)