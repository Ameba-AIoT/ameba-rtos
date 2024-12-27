set(CSRC)

list(
	APPEND CSRC 
	${RTK_OT_BR}/rtk_otbr.c 
	${RTK_OT_BR}/rtk_otbr_netif.c 
	${RTK_OT_BR}/rtk_otbr_udp.c 
	${RTK_OT_BR}/rtk_otbr_infra_if.c 
	${RTK_OT_BR}/rtk_otbr_multicast_routing.c 
	${RTK_OT_BR}/rtk_otbr_nat64.c 
	${RTK_OT_BR}/rtk_otbr_nat66.c 
	${RTK_OT_BR}/rtk_otbr_border_agent.c 
	${RTK_OT_BR}/rtk_otbr_srp_server.c 
	${RTK_OT_BR}/rtk_otbr_discovery.c 
	${RTK_OT_BR}/rtk_otbr_resolver.c 
	${RTK_OT_BR}/rtk_otbr_nd_proxying.c 
	${RTK_OT_BR}/rtk_otbr_backbone_agent.c 
	${RTK_OT_BR}/rtk_otbr_trel.c 
	${RTK_OT_BR}/rtk_otbr_osif.c 
	${RTK_OT_BR}/rtk_otbr_tcpip_utils.c 
	${RTK_OT_BR}/rtk_otbr_workflow.c 
	${RTK_OT_BR}/rtk_otbr_cli_extension.c 
	${RTK_OT_BR}/mdns/mdns.c 
	${RTK_OT_BR}/mdns/mdns_domain.c 
	${RTK_OT_BR}/mdns/mdns_out.c
)

ameba_app_library(otbr)
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_DEFINES})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})