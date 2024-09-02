set(THREADDIR ${BASEDIR}/component/wpan/openthread)
set(TIMER ${BASEDIR}/component/wpan/platform/timer)
set(FLASH  ${BASEDIR}/component/wpan/platform/flash)
set(PLATFORM_CONFIG ${BASEDIR}/component/wpan/platform/config)
set(RTK_OT_SUPPORT ${BASEDIR}/component/wpan/openthread/rtk_ot_support)
set(RTK_OT_BR ${BASEDIR}/component/wpan/openthread/rtk_otbr)

set(DIR ${THREADDIR}/openthread/src/core)
set(CLI_DIR ${THREADDIR}/openthread/src/cli)
set(EX_UTILS_DIR ${THREADDIR}/openthread/examples/platforms/utils)
set(API_DIR ${DIR}/api)
set(BACKROUTER_DIR ${DIR}/backbone_router)
set(BR_DIR ${DIR}/border_router)
set(COAP_DIR ${DIR}/coap)
set(COMMON_DIR ${DIR}/common)
set(CRYPTO_DIR ${DIR}/crypto)
set(DIAGS_DIR ${DIR}/diags)
set(MAC_DIR ${DIR}/mac)
set(MESHCOP_DIR ${DIR}/meshcop)
set(NET_DIR ${DIR}/net)
set(RADIO_DIR ${DIR}/radio)
set(THREAD_DIR ${DIR}/thread)
set(UTILS_DIR ${DIR}/utils)

execute_process(
    COMMAND ${GIT_EXECUTABLE} apply ./../tcplp_dup_name.ptach
    WORKING_DIRECTORY ${THREADDIR}/openthread
    RESULT_VARIABLE ERROR_CODE
)

if(ERROR_CODE)
    message(FATAL_ERROR "git apply tcplp_dup_name.ptach error! " ${ERROR_CODE})
endif()

set(MODULE_C_DEFINES CONFIG_802154_THREAD_RADIO_RCP_EN)
set(MODULE_CPP_OPTIONS -Wno-unused-parameter)

set(CSRC)
set(CPPSRC)

set(
    MODULE_IFLAGS
    ${BASEDIR}/component/soc/amebadplus/fwlib/include
    ${BASEDIR}/component/component/os/os_dep/
    ${TIMER}/
    ${FLASH}/
    ${PLATFORM_CONFIG}/
    ${THREADDIR}/config/
    ${THREADDIR}/rtk_otbr/
    ${THREADDIR}/openthread/src/
    ${THREADDIR}/openthread/src/core/
    ${THREADDIR}/openthread/src/core/config/
    ${THREADDIR}/openthread/include/
    ${THREADDIR}/openthread/examples/
    ${THREADDIR}/openthread/examples/platforms/
    ${THREADDIR}/openthread/examples/platforms/utils/

    ${THREADDIR}/openthread/cli/
)

if(CONFIG_802154_THREAD_BORDER_ROUTER_EN)
    list(APPEND MODULE_C_DEFINES OPENTHREAD_FTD)
    set(RTK_OT_BR ${BASEDIR}/component/wpan/openthread/rtk_otbr)
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
		${RTK_OT_BR}/rtk_otbr_cli_extension.c 
		${RTK_OT_BR}/rtk_otbr_trel.c 
		${RTK_OT_BR}/rtk_otbr_osif.c 
		${RTK_OT_BR}/rtk_otbr_tcpip_utils.c 
		${RTK_OT_BR}/rtk_otbr_workflow.c 
		${RTK_OT_BR}/mdns/mdns.c 
		${RTK_OT_BR}/mdns/mdns_domain.c 
		${RTK_OT_BR}/mdns/mdns_out.c
    )
endif()

if(CONFIG_802154_THREAD_FTD_EN)
    list(APPEND MODULE_C_DEFINES OPENTHREAD_FTD)
endif()

if(CONFIG_802154_THREAD_MTD_EN)
    list(APPEND MODULE_C_DEFINES OPENTHREAD_MTD)
endif()

list(
    APPEND CSRC 
    #OpenThread example
    ${EX_UTILS_DIR}/debug_uart.c
    ${EX_UTILS_DIR}/settings_ram.c
    ${EX_UTILS_DIR}/soft_source_match_table.c

    #OpenThread third_party tcpIp
    ${THREADDIR}/openthread/third_party/tcplp/bsdtcp/cc/cc_newreno.c
    ${THREADDIR}/openthread/third_party/tcplp/bsdtcp/tcp_fastopen.c
    ${THREADDIR}/openthread/third_party/tcplp/bsdtcp/tcp_input.c
    ${THREADDIR}/openthread/third_party/tcplp/bsdtcp/tcp_output.c
    ${THREADDIR}/openthread/third_party/tcplp/bsdtcp/tcp_reass.c
    ${THREADDIR}/openthread/third_party/tcplp/bsdtcp/tcp_sack.c
    ${THREADDIR}/openthread/third_party/tcplp/bsdtcp/tcp_subr.c
    ${THREADDIR}/openthread/third_party/tcplp/bsdtcp/tcp_timer.c
    ${THREADDIR}/openthread/third_party/tcplp/bsdtcp/tcp_timewait.c
    ${THREADDIR}/openthread/third_party/tcplp/bsdtcp/tcp_usrreq.c
    ${THREADDIR}/openthread/third_party/tcplp/lib/bitmap.c
    ${THREADDIR}/openthread/third_party/tcplp/lib/cbuf.c
    ${THREADDIR}/openthread/third_party/tcplp/lib/lbuf.c

    #OpenThread platform
    ${RTK_OT_SUPPORT}/rtk_ot_alarm.c 
    ${RTK_OT_SUPPORT}/rtk_ot_entropy.c 
    ${RTK_OT_SUPPORT}/rtk_ot_flash.c 
    ${RTK_OT_SUPPORT}/rtk_ot_logging.c 
    ${RTK_OT_SUPPORT}/rtk_ot_misc.c 
    ${RTK_OT_SUPPORT}/rtk_ot_system.c 
    ${RTK_OT_SUPPORT}/rtk_ot_crypto.c 
    ${RTK_OT_SUPPORT}/rtk_ot_uart.c 
    ${RTK_OT_SUPPORT}/rtk_ot_settings.c 
    ${RTK_OT_SUPPORT}/rtk_ot_test.c 
    ${RTK_OT_SUPPORT}/rtk_ot_cli_uart.c

    #OpenThread lib platform
    ${THREADDIR}/openthread/src/lib/platform/exit_code.c

    #OpenThread lib spinel
    ${THREADDIR}/openthread/src/lib/spinel/spinel.c
)

list(
    APPEND CPPSRC 
    #OpenThread example
    ${EX_UTILS_DIR}/link_metrics.cpp
    ${EX_UTILS_DIR}/mac_frame.cpp
    ${EX_UTILS_DIR}/otns_utils.cpp

    #OpenThread CLI
    ${CLI_DIR}/cli.cpp
    ${CLI_DIR}/cli_bbr.cpp
    ${CLI_DIR}/cli_br.cpp
    ${CLI_DIR}/cli_coap.cpp
    ${CLI_DIR}/cli_coap_secure.cpp
    ${CLI_DIR}/cli_commissioner.cpp
    ${CLI_DIR}/cli_dataset.cpp
    ${CLI_DIR}/cli_dns.cpp
    #${CLI_DIR}/cli_history.cpp
    ${CLI_DIR}/cli_joiner.cpp
    ${CLI_DIR}/cli_mac_filter.cpp
    ${CLI_DIR}/cli_network_data.cpp
    ${CLI_DIR}/cli_output.cpp
    ${CLI_DIR}/cli_srp_client.cpp
    ${CLI_DIR}/cli_srp_server.cpp
    ${CLI_DIR}/cli_tcp.cpp
    ${CLI_DIR}/cli_udp.cpp

    #OpenThread src API
    ${API_DIR}/backbone_router_api.cpp
    ${API_DIR}/backbone_router_ftd_api.cpp
    ${API_DIR}/border_agent_api.cpp
    ${API_DIR}/border_router_api.cpp
    ${API_DIR}/border_routing_api.cpp
    ${API_DIR}/channel_manager_api.cpp
    ${API_DIR}/channel_monitor_api.cpp
    ${API_DIR}/child_supervision_api.cpp
    ${API_DIR}/coap_api.cpp
    ${API_DIR}/coap_secure_api.cpp
    ${API_DIR}/commissioner_api.cpp
    ${API_DIR}/crypto_api.cpp
    ${API_DIR}/dataset_api.cpp
    ${API_DIR}/dataset_ftd_api.cpp
    ${API_DIR}/dataset_updater_api.cpp
    ${API_DIR}/diags_api.cpp
    ${API_DIR}/dns_api.cpp
    ${API_DIR}/dns_server_api.cpp
    ${API_DIR}/error_api.cpp
    ${API_DIR}/heap_api.cpp
    #${API_DIR}/history_tracker_api.cpp
    ${API_DIR}/icmp6_api.cpp
    ${API_DIR}/instance_api.cpp
    ${API_DIR}/ip6_api.cpp
    ${API_DIR}/jam_detection_api.cpp
    ${API_DIR}/joiner_api.cpp
    ${API_DIR}/link_api.cpp
    ${API_DIR}/link_metrics_api.cpp
    ${API_DIR}/link_raw_api.cpp
    ${API_DIR}/logging_api.cpp
    ${API_DIR}/mesh_diag_api.cpp
    ${API_DIR}/message_api.cpp
    ${API_DIR}/multi_radio_api.cpp
    ${API_DIR}/nat64_api.cpp
    ${API_DIR}/netdata_api.cpp
    ${API_DIR}/netdata_publisher_api.cpp
    ${API_DIR}/netdiag_api.cpp
    ${API_DIR}/network_time_api.cpp
    ${API_DIR}/ping_sender_api.cpp
    ${API_DIR}/radio_stats_api.cpp
    ${API_DIR}/random_crypto_api.cpp
    ${API_DIR}/random_noncrypto_api.cpp
    ${API_DIR}/server_api.cpp
    ${API_DIR}/sntp_api.cpp
    ${API_DIR}/srp_client_api.cpp
    ${API_DIR}/srp_client_buffers_api.cpp
    ${API_DIR}/srp_server_api.cpp
    ${API_DIR}/tasklet_api.cpp
    ${API_DIR}/tcp_api.cpp
    ${API_DIR}/tcp_ext_api.cpp
    ${API_DIR}/thread_api.cpp
    ${API_DIR}/thread_ftd_api.cpp
    ${API_DIR}/udp_api.cpp
    ${API_DIR}/trel_api.cpp

    #OpenThread src BACKBONE_ROUTER
    ${BACKROUTER_DIR}/backbone_tmf.cpp
    ${BACKROUTER_DIR}/bbr_leader.cpp
    ${BACKROUTER_DIR}/bbr_local.cpp
    ${BACKROUTER_DIR}/bbr_manager.cpp
    ${BACKROUTER_DIR}/multicast_listeners_table.cpp
    ${BACKROUTER_DIR}/ndproxy_table.cpp

    #OpenThread src BORDER_ROUTER
    ${BR_DIR}/infra_if.cpp
    ${BR_DIR}/routing_manager.cpp

    #OpenThread src COAP
    ${COAP_DIR}/coap.cpp
    ${COAP_DIR}/coap_message.cpp
    ${COAP_DIR}/coap_secure.cpp

    #OpenThread src COMMON
    ${COMMON_DIR}/appender.cpp
    ${COMMON_DIR}/binary_search.cpp
    ${COMMON_DIR}/crc16.cpp
    ${COMMON_DIR}/data.cpp
    ${COMMON_DIR}/error.cpp
    ${COMMON_DIR}/frame_builder.cpp
    ${COMMON_DIR}/frame_data.cpp
    ${COMMON_DIR}/heap.cpp
    ${COMMON_DIR}/heap_data.cpp
    ${COMMON_DIR}/heap_string.cpp
    ${COMMON_DIR}/instance.cpp
    ${COMMON_DIR}/log.cpp
    ${COMMON_DIR}/message.cpp
    ${COMMON_DIR}/notifier.cpp
    ${COMMON_DIR}/preference.cpp
    ${COMMON_DIR}/random.cpp
    ${COMMON_DIR}/settings.cpp
    ${COMMON_DIR}/string.cpp
    ${COMMON_DIR}/tasklet.cpp
    ${COMMON_DIR}/time_ticker.cpp
    ${COMMON_DIR}/timer.cpp
    ${COMMON_DIR}/tlvs.cpp
    ${COMMON_DIR}/trickle_timer.cpp
    ${COMMON_DIR}/uptime.cpp

    #OpenThread src CRYPTO
    ${CRYPTO_DIR}/aes_ccm.cpp
    ${CRYPTO_DIR}/aes_ecb.cpp
    ${CRYPTO_DIR}/crypto_platform.cpp
    ${CRYPTO_DIR}/hkdf_sha256.cpp
    ${CRYPTO_DIR}/hmac_sha256.cpp
    ${CRYPTO_DIR}/mbedtls.cpp
    ${CRYPTO_DIR}/sha256.cpp
    ${CRYPTO_DIR}/storage.cpp

    #OpenThread src DIAGS
    ${DIAGS_DIR}/factory_diags.cpp

    #OpenThread src MAC
    ${MAC_DIR}/channel_mask.cpp
    ${MAC_DIR}/data_poll_handler.cpp
    ${MAC_DIR}/data_poll_sender.cpp
    ${MAC_DIR}/link_raw.cpp
    ${MAC_DIR}/mac.cpp
    ${MAC_DIR}/mac_filter.cpp
    ${MAC_DIR}/mac_frame.cpp
    ${MAC_DIR}/mac_links.cpp
    ${MAC_DIR}/mac_types.cpp
    ${MAC_DIR}/sub_mac.cpp
    ${MAC_DIR}/sub_mac_callbacks.cpp

    #OpenThread src MESHCOP
    ${MESHCOP_DIR}/announce_begin_client.cpp
    ${MESHCOP_DIR}/border_agent.cpp
    ${MESHCOP_DIR}/commissioner.cpp
    ${MESHCOP_DIR}/dataset.cpp
    ${MESHCOP_DIR}/dataset_local.cpp
    ${MESHCOP_DIR}/dataset_manager.cpp
    ${MESHCOP_DIR}/dataset_manager_ftd.cpp
    ${MESHCOP_DIR}/dataset_updater.cpp
    ${MESHCOP_DIR}/dtls.cpp
    ${MESHCOP_DIR}/energy_scan_client.cpp
    ${MESHCOP_DIR}/extended_panid.cpp
    ${MESHCOP_DIR}/joiner.cpp
    ${MESHCOP_DIR}/joiner_router.cpp
    ${MESHCOP_DIR}/meshcop.cpp
    ${MESHCOP_DIR}/meshcop_leader.cpp
    ${MESHCOP_DIR}/meshcop_tlvs.cpp
    ${MESHCOP_DIR}/network_name.cpp
    ${MESHCOP_DIR}/panid_query_client.cpp
    ${MESHCOP_DIR}/timestamp.cpp

    #OpenThread src NET
    ${NET_DIR}/checksum.cpp
    ${NET_DIR}/dhcp6_client.cpp
    ${NET_DIR}/dhcp6_server.cpp
    ${NET_DIR}/dns_client.cpp
    ${NET_DIR}/dns_dso.cpp
    ${NET_DIR}/dns_platform.cpp
    ${NET_DIR}/dns_types.cpp
    ${NET_DIR}/dnssd_server.cpp
    ${NET_DIR}/icmp6.cpp
    ${NET_DIR}/ip4_types.cpp
    ${NET_DIR}/ip6.cpp
    ${NET_DIR}/ip6_address.cpp
    ${NET_DIR}/ip6_filter.cpp
    ${NET_DIR}/ip6_headers.cpp
    ${NET_DIR}/ip6_mpl.cpp
    ${NET_DIR}/nat64_translator.cpp
    ${NET_DIR}/nd_agent.cpp
    ${NET_DIR}/nd6.cpp
    ${NET_DIR}/netif.cpp
    ${NET_DIR}/sntp_client.cpp
    ${NET_DIR}/socket.cpp
    ${NET_DIR}/srp_client.cpp
    ${NET_DIR}/srp_server.cpp
    ${NET_DIR}/tcp6.cpp
    ${NET_DIR}/tcp6_ext.cpp
    ${NET_DIR}/udp6.cpp


    #OpenThread src RADIO
    ${RADIO_DIR}/radio.cpp
    ${RADIO_DIR}/radio_callbacks.cpp
    ${RADIO_DIR}/radio_platform.cpp
    ${RADIO_DIR}/trel_interface.cpp
    ${RADIO_DIR}/trel_link.cpp
    ${RADIO_DIR}/trel_packet.cpp

    #OpenThread src THREAD
    ${THREAD_DIR}/address_resolver.cpp
    ${THREAD_DIR}/announce_begin_server.cpp
    ${THREAD_DIR}/announce_sender.cpp
    ${THREAD_DIR}/anycast_locator.cpp
    ${THREAD_DIR}/child_supervision.cpp
    ${THREAD_DIR}/child_table.cpp
    ${THREAD_DIR}/csl_tx_scheduler.cpp
    ${THREAD_DIR}/discover_scanner.cpp
    ${THREAD_DIR}/dua_manager.cpp
    ${THREAD_DIR}/energy_scan_server.cpp
    ${THREAD_DIR}/indirect_sender.cpp
    ${THREAD_DIR}/key_manager.cpp
    ${THREAD_DIR}/link_metrics.cpp
    ${THREAD_DIR}/link_metrics_types.cpp
    ${THREAD_DIR}/link_quality.cpp
    ${THREAD_DIR}/lowpan.cpp
    ${THREAD_DIR}/mesh_forwarder.cpp
    ${THREAD_DIR}/mesh_forwarder_ftd.cpp
    ${THREAD_DIR}/mesh_forwarder_mtd.cpp
    ${THREAD_DIR}/mle.cpp
    ${THREAD_DIR}/mle_router.cpp
    ${THREAD_DIR}/mle_tlvs.cpp
    ${THREAD_DIR}/mle_types.cpp
    ${THREAD_DIR}/mlr_manager.cpp
    ${THREAD_DIR}/neighbor_table.cpp
    ${THREAD_DIR}/network_data.cpp
    ${THREAD_DIR}/network_data_leader.cpp
    ${THREAD_DIR}/network_data_leader_ftd.cpp
    ${THREAD_DIR}/network_data_local.cpp
    ${THREAD_DIR}/network_data_notifier.cpp
    ${THREAD_DIR}/network_data_publisher.cpp
    ${THREAD_DIR}/network_data_service.cpp
    ${THREAD_DIR}/network_data_tlvs.cpp
    ${THREAD_DIR}/network_data_types.cpp
    ${THREAD_DIR}/network_diagnostic.cpp
    ${THREAD_DIR}/network_diagnostic_tlvs.cpp
    ${THREAD_DIR}/panid_query_server.cpp
    ${THREAD_DIR}/radio_selector.cpp
    ${THREAD_DIR}/router_table.cpp
    ${THREAD_DIR}/src_match_controller.cpp
    ${THREAD_DIR}/thread_netif.cpp
    ${THREAD_DIR}/time_sync_service.cpp
    ${THREAD_DIR}/tmf.cpp
    ${THREAD_DIR}/child.cpp
    ${THREAD_DIR}/neighbor.cpp
    ${THREAD_DIR}/router.cpp
    ${THREAD_DIR}/uri_paths.cpp

    #OpenThread src UTILS
    ${UTILS_DIR}/channel_manager.cpp
    ${UTILS_DIR}/channel_monitor.cpp
    ${UTILS_DIR}/flash.cpp
    ${UTILS_DIR}/heap.cpp
    #${UTILS_DIR}/history_tracker.cpp
    ${UTILS_DIR}/jam_detector.cpp
    ${UTILS_DIR}/link_metrics_manager.cpp
    ${UTILS_DIR}/mesh_diag.cpp
    ${UTILS_DIR}/otns.cpp
    ${UTILS_DIR}/parse_cmdline.cpp
    ${UTILS_DIR}/ping_sender.cpp
    ${UTILS_DIR}/power_calibration.cpp
    ${UTILS_DIR}/slaac_address.cpp
    ${UTILS_DIR}/srp_client_buffers.cpp

    #OpenThread lib hdlc
    ${THREADDIR}/openthread/src/lib/hdlc/hdlc.cpp

    #OpenThread lib spinel
    ${THREADDIR}/openthread/src/lib/spinel/spinel_buffer.cpp
    ${THREADDIR}/openthread/src/lib/spinel/spinel_decoder.cpp
    ${THREADDIR}/openthread/src/lib/spinel/spinel_encoder.cpp

    ${THREADDIR}/openthread/src/lib/spinel/radio_spinel.cpp
    #OpenThread radio
    ${RTK_OT_SUPPORT}/radio/rcp_radio/rtk_ot_radio.cpp
    ${RTK_OT_SUPPORT}/radio/rcp_radio/rtk_ot_hdlc_interface.cpp
)

ameba_app_library(openthread)
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC} ${CPPSRC})
target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_DEFINES})
target_compile_options(${CURRENT_LIB_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${MODULE_CPP_OPTIONS}>)
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})