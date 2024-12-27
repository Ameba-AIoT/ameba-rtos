if (__INCLUDED_${d_MCU_PROJECT_NAME}_NETWORK_BUILD_CMAKE__)
    return()
else()
    set(__INCLUDED_${d_MCU_PROJECT_NAME}_NETWORK_BUILD_CMAKE__ TRUE)
endif()
if(NOT d_AMEBA_REALSE)
    add_library(src_${d_MCU_PROJECT_NAME}_network_ipcoap INTERFACE)
    add_library(src_${d_MCU_PROJECT_NAME}_network_ipnat INTERFACE)
    add_library(src_${d_MCU_PROJECT_NAME}_network_ipdnrd INTERFACE)
    add_library(src_${d_MCU_PROJECT_NAME}_network_coap INTERFACE)
    add_library(src_${d_MCU_PROJECT_NAME}_network_httpc INTERFACE)
    add_library(src_${d_MCU_PROJECT_NAME}_network_httpd INTERFACE)
    add_library(src_${d_MCU_PROJECT_NAME}_network_tftp INTERFACE)
    add_library(src_${d_MCU_PROJECT_NAME}_network_websocket INTERFACE)
    add_library(src_${d_MCU_PROJECT_NAME}_network_rtsp INTERFACE)
endif()
add_library(src_${d_MCU_PROJECT_NAME}_network_http2 INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_network_lwip INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_network_mqtt_client INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_network_mqtt_packet INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_network_sram INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_network_xip INTERFACE)
#--------------------------#
ameba_target_set(src_${d_MCU_PROJECT_NAME}_network_coap p_SCOPE interface
    p_INCLUDES
        ${c_CMPT_NETWORK_DIR}/libcoap/include
    p_SOURCES
        ${c_CMPT_NETWORK_DIR}/libcoap/async.c
        ${c_CMPT_NETWORK_DIR}/libcoap/block.c
        ${c_CMPT_NETWORK_DIR}/libcoap/coap_debug.c
        ${c_CMPT_NETWORK_DIR}/libcoap/coap_list.c
        ${c_CMPT_NETWORK_DIR}/libcoap/coap_net.c
        ${c_CMPT_NETWORK_DIR}/libcoap/encode.c
        ${c_CMPT_NETWORK_DIR}/libcoap/hashkey.c
        ${c_CMPT_NETWORK_DIR}/libcoap/option.c
        ${c_CMPT_NETWORK_DIR}/libcoap/pdu.c
        ${c_CMPT_NETWORK_DIR}/libcoap/resource.c
        ${c_CMPT_NETWORK_DIR}/libcoap/str.c
        ${c_CMPT_NETWORK_DIR}/libcoap/subscribe.c
        ${c_CMPT_NETWORK_DIR}/libcoap/uri.c
)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_network_http2 p_SCOPE interface
    ${c_CMPT_NETWORK_DIR}/http2/nghttp2-${v_HTTP2_VER}/nghttp2_buf.c
    ${c_CMPT_NETWORK_DIR}/http2/nghttp2-${v_HTTP2_VER}/nghttp2_callbacks.c
    ${c_CMPT_NETWORK_DIR}/http2/nghttp2-${v_HTTP2_VER}/nghttp2_debug.c
    ${c_CMPT_NETWORK_DIR}/http2/nghttp2-${v_HTTP2_VER}/nghttp2_frame.c
    ${c_CMPT_NETWORK_DIR}/http2/nghttp2-${v_HTTP2_VER}/nghttp2_hd.c
    ${c_CMPT_NETWORK_DIR}/http2/nghttp2-${v_HTTP2_VER}/nghttp2_hd_huffman.c
    ${c_CMPT_NETWORK_DIR}/http2/nghttp2-${v_HTTP2_VER}/nghttp2_hd_huffman_data.c
    ${c_CMPT_NETWORK_DIR}/http2/nghttp2-${v_HTTP2_VER}/nghttp2_helper.c
    ${c_CMPT_NETWORK_DIR}/http2/nghttp2-${v_HTTP2_VER}/nghttp2_http.c
    ${c_CMPT_NETWORK_DIR}/http2/nghttp2-${v_HTTP2_VER}/nghttp2_map.c
    ${c_CMPT_NETWORK_DIR}/http2/nghttp2-${v_HTTP2_VER}/nghttp2_mem.c
    ${c_CMPT_NETWORK_DIR}/http2/nghttp2-${v_HTTP2_VER}/nghttp2_option.c
    ${c_CMPT_NETWORK_DIR}/http2/nghttp2-${v_HTTP2_VER}/nghttp2_outbound_item.c
    ${c_CMPT_NETWORK_DIR}/http2/nghttp2-${v_HTTP2_VER}/nghttp2_pq.c
    ${c_CMPT_NETWORK_DIR}/http2/nghttp2-${v_HTTP2_VER}/nghttp2_priority_spec.c
    ${c_CMPT_NETWORK_DIR}/http2/nghttp2-${v_HTTP2_VER}/nghttp2_queue.c
    ${c_CMPT_NETWORK_DIR}/http2/nghttp2-${v_HTTP2_VER}/nghttp2_rcbuf.c
    ${c_CMPT_NETWORK_DIR}/http2/nghttp2-${v_HTTP2_VER}/nghttp2_session.c
    ${c_CMPT_NETWORK_DIR}/http2/nghttp2-${v_HTTP2_VER}/nghttp2_stream.c
    ${c_CMPT_NETWORK_DIR}/http2/nghttp2-${v_HTTP2_VER}/nghttp2_submit.c
    ${c_CMPT_NETWORK_DIR}/http2/nghttp2-${v_HTTP2_VER}/nghttp2_version.c
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_network_http2 p_SCOPE interface
    ${c_CMPT_NETWORK_DIR}/http2/nghttp2-${v_HTTP2_VER}/includes
)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_network_http2 p_SCOPE interface -Wno-implicit-fallthrough)

#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_network_httpc p_SCOPE interface
    ${c_CMPT_NETWORK_DIR}/httpc/httpc_core.c
    ${c_CMPT_NETWORK_DIR}/httpc/httpc_msg.c
    ${c_CMPT_NETWORK_DIR}/httpc/httpc_util.c
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_network_httpc p_SCOPE interface
    ${c_CMPT_NETWORK_DIR}/httpc
)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_network_httpd p_SCOPE interface
    ${c_CMPT_NETWORK_DIR}/httpd/httpd_core.c
    ${c_CMPT_NETWORK_DIR}/httpd/httpd_msg.c
    ${c_CMPT_NETWORK_DIR}/httpd/httpd_util.c
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_network_httpd p_SCOPE interface
    ${c_CMPT_NETWORK_DIR}/httpd
)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_network_ipdnrd p_SCOPE interface
    ${c_CMPT_NETWORK_DIR}/dnrd/src/common.c
    ${c_CMPT_NETWORK_DIR}/dnrd/src/dnrd.c
    ${c_CMPT_NETWORK_DIR}/dnrd/src/dns.c
    ${c_CMPT_NETWORK_DIR}/dnrd/src/lib.c
    ${c_CMPT_NETWORK_DIR}/dnrd/src/master.c
    ${c_CMPT_NETWORK_DIR}/dnrd/src/query.c
    ${c_CMPT_NETWORK_DIR}/dnrd/src/relay.c
    ${c_CMPT_NETWORK_DIR}/dnrd/src/udp.c
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_network_ipdnrd p_SCOPE interface
    ${c_CMPT_NETWORK_DIR}/dnrd/src
)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_network_ipnat p_SCOPE interface
    ${c_LWIP_DIR}/src/core/ipv4/ip_nat/lwip_ip4nat.c
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_network_ipnat p_SCOPE interface
    ${c_LWIP_DIR}/src/core/ipv4/ip_nat
)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_network_ipnat p_SCOPE interface
    IP_FORWARD
    IP_NAT
)
#--------------------------#

ameba_target_sources(src_${d_MCU_PROJECT_NAME}_network_lwip p_SCOPE interface
    ${c_CMPT_NETWORK_DIR}/sntp/sntp.c
    ${c_CMPT_NETWORK_DIR}/websocket/wsserver_tls.c
    ${c_CMPT_NETWORK_DIR}/websocket/wsclient_tls.c
    ${c_CMPT_NETWORK_DIR}/dhcp/dhcps.c
    ${c_CMPT_NETWORK_DIR}/httpc/httpc_tls.c
    ${c_CMPT_NETWORK_DIR}/httpd/httpd_tls.c
    ${c_CMPT_NETWORK_DIR}/mDNS/mDNSPlatform.c
    #${c_CMPT_NETWORK_DIR}/coap/sn_coap_ameba_port.c
    #${c_CMPT_NETWORK_DIR}/coap/sn_coap_builder.c
    #${c_CMPT_NETWORK_DIR}/coap/sn_coap_header_check.c
    #${c_CMPT_NETWORK_DIR}/coap/sn_coap_parser.c
    #${c_CMPT_NETWORK_DIR}/coap/sn_coap_protocol.c

    ${c_LWIP_DIR}/src/core/ipv4/dhcp.c
    ${c_LWIP_DIR}/src/core/ipv6/dhcp6.c
    ${c_LWIP_DIR}/src/core/dns.c
    ${c_LWIP_DIR}/src/core/init.c
    ${c_LWIP_DIR}/src/core/ipv4/autoip.c
    ${c_LWIP_DIR}/src/core/ipv4/icmp.c
    ${c_LWIP_DIR}/src/core/ipv6/icmp6.c
    ${c_LWIP_DIR}/src/core/ipv6/inet6.c
    ${c_LWIP_DIR}/src/core/ipv6/mld6.c
    ${c_LWIP_DIR}/src/core/ipv6/nd6.c
    ${c_LWIP_DIR}/src/api/err.c
    ${c_LWIP_DIR}/src/core/stats.c
    ${c_LWIP_DIR}/src/core/sys.c
    ${c_LWIP_DIR}/src/core/tcp_in.c
    ${c_LWIP_DIR}/src/core/tcp_out.c
    ${c_LWIP_DIR}/src/core/def.c
    ${c_LWIP_DIR}/src/core/ipv4/igmp.c
    ${c_LWIP_DIR}/src/core/pbuf.c
    ${c_LWIP_DIR}/src/core/inet_chksum.c
    ${c_LWIP_DIR}/src/api/api_msg.c
    ${c_LWIP_DIR}/src/api/netbuf.c
    ${c_LWIP_DIR}/port/realtek/freertos/sys_arch.c
    ${c_LWIP_DIR}/port/realtek/freertos/ethernetif.c
    ${c_LWIP_DIR}/src/core/timeouts.c
    ${c_LWIP_DIR}/src/core/raw.c
    ${c_LWIP_DIR}/src/core/mem.c
    ${c_LWIP_DIR}/src/core/memp.c
    ${c_LWIP_DIR}/src/core/netif.c
    ${c_LWIP_DIR}/src/core/ip.c
    ${c_LWIP_DIR}/src/core/ipv4/ip4.c
    ${c_LWIP_DIR}/src/core/ipv4/ip4_addr.c
    ${c_LWIP_DIR}/src/core/ipv4/ip4_frag.c
    ${c_LWIP_DIR}/src/core/ipv6/ip6.c
    ${c_LWIP_DIR}/src/core/ipv6/ip6_addr.c
    ${c_LWIP_DIR}/src/core/ipv6/ip6_frag.c
    ${c_LWIP_DIR}/src/core/tcp.c
    ${c_LWIP_DIR}/src/core/udp.c
    ${c_LWIP_DIR}/src/api/sockets.c
    ${c_LWIP_DIR}/src/api/tcpip.c
    ${c_LWIP_DIR}/src/netif/ethernet.c
    ${c_LWIP_DIR}/src/core/ipv4/etharp.c
    ${c_LWIP_DIR}/src/core/ipv6/ethip6.c
    ${c_LWIP_DIR}/src/api/netdb.c
    ${c_LWIP_DIR}/src/api/api_lib.c
    ${c_LWIP_DIR}/src/api/netifapi.c
    ${c_LWIP_DIR}/src/apps/mdns/mdns.c

    ${c_CMPT_LWIP_DIR}/api/lwip_netconf.c
)
ameba_target_include_if(CONFIG_802154_THREAD_BORDER_ROUTER_EN src_${d_MCU_PROJECT_NAME}_network_lwip p_SCOPE interface
    ${c_COMPONENT_DIR}/wpan/openthread/rtk_otbr
)
#--------------------------#

ameba_target_sources(src_${d_MCU_PROJECT_NAME}_network_mqtt_client p_SCOPE interface
        ${c_CMPT_NETWORK_DIR}/mqtt/MQTTClient/MQTTClient.c
        ${c_CMPT_NETWORK_DIR}/mqtt/MQTTClient/MQTTFreertos.c
)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_network_mqtt_packet p_SCOPE interface
    ${c_CMPT_NETWORK_DIR}/mqtt/MQTTPacket/MQTTConnectClient.c
    ${c_CMPT_NETWORK_DIR}/mqtt/MQTTPacket/MQTTConnectServer.c
    ${c_CMPT_NETWORK_DIR}/mqtt/MQTTPacket/MQTTDeserializePublish.c
    ${c_CMPT_NETWORK_DIR}/mqtt/MQTTPacket/MQTTFormat.c
    ${c_CMPT_NETWORK_DIR}/mqtt/MQTTPacket/MQTTPacket.c
    ${c_CMPT_NETWORK_DIR}/mqtt/MQTTPacket/MQTTSerializePublish.c
    ${c_CMPT_NETWORK_DIR}/mqtt/MQTTPacket/MQTTSubscribeClient.c
    ${c_CMPT_NETWORK_DIR}/mqtt/MQTTPacket/MQTTSubscribeServer.c
    ${c_CMPT_NETWORK_DIR}/mqtt/MQTTPacket/MQTTUnsubscribeClient.c
    ${c_CMPT_NETWORK_DIR}/mqtt/MQTTPacket/MQTTUnsubscribeServer.c
)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_network_mqtt_packet p_SCOPE interface -Wno-implicit-fallthrough)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_network_rtsp p_SCOPE interface
    ${c_CMPT_NETWORK_DIR}/rtsp/rtp_api.c
    ${c_CMPT_NETWORK_DIR}/rtsp/rtsp_api.c
    ${c_CMPT_NETWORK_DIR}/rtsp/sdp.c
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_network_rtsp p_SCOPE interface
    ${c_CMPT_NETWORK_DIR}/rtsp
)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_network_sram p_SCOPE interface
    ${c_LWIP_DIR}/src/core/tcp_in.c
    ${c_LWIP_DIR}/src/core/tcp_out.c
    ${c_LWIP_DIR}/src/core/def.c
    ${c_LWIP_DIR}/src/core/ipv4/igmp.c
    ${c_LWIP_DIR}/src/core/pbuf.c
    ${c_LWIP_DIR}/src/core/inet_chksum.c
    ${c_LWIP_DIR}/src/api/api_msg.c
    ${c_LWIP_DIR}/src/api/netbuf.c
    ${c_LWIP_DIR}/port/realtek/freertos/sys_arch.c
    ${c_LWIP_DIR}/port/realtek/freertos/ethernetif.c
    ${c_LWIP_DIR}/src/core/timeouts.c
    ${c_LWIP_DIR}/src/core/raw.c
    ${c_LWIP_DIR}/src/core/mem.c
    ${c_LWIP_DIR}/src/core/memp.c
    ${c_LWIP_DIR}/src/core/netif.c
    ${c_LWIP_DIR}/src/core/ip.c
    ${c_LWIP_DIR}/src/core/ipv4/ip4.c
    ${c_LWIP_DIR}/src/core/ipv4/ip4_addr.c
    ${c_LWIP_DIR}/src/core/ipv4/ip4_frag.c
    ${c_LWIP_DIR}/src/core/ipv6/ip6.c
    ${c_LWIP_DIR}/src/core/ipv6/ip6_addr.c
    ${c_LWIP_DIR}/src/core/ipv6/ip6_frag.c
    ${c_LWIP_DIR}/src/core/tcp.c
    ${c_LWIP_DIR}/src/core/udp.c
    ${c_LWIP_DIR}/src/api/sockets.c
    ${c_LWIP_DIR}/src/api/tcpip.c
    ${c_LWIP_DIR}/src/netif/ethernet.c
    ${c_LWIP_DIR}/src/core/ipv4/etharp.c
    ${c_LWIP_DIR}/src/core/ipv6/ethip6.c
    ${c_LWIP_DIR}/src/api/netdb.c
    ${c_LWIP_DIR}/src/api/api_lib.c
    ${c_LWIP_DIR}/src/api/netifapi.c
    ${c_LWIP_DIR}/src/apps/mdns/mdns.c
)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_network_tftp p_SCOPE interface
    ${c_CMPT_NETWORK_DIR}/tftp/tftp_client.c
    ${c_CMPT_NETWORK_DIR}/tftp/tftp_server.c
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_network_tftp p_SCOPE interface
    ${c_CMPT_NETWORK_DIR}/tftp
)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_network_websocket p_SCOPE interface
    ${c_CMPT_NETWORK_DIR}/websocket/libwsclient.c
    ${c_CMPT_NETWORK_DIR}/websocket/ws_server_msg.c
    ${c_CMPT_NETWORK_DIR}/websocket/wsclient_api.c
    ${c_CMPT_NETWORK_DIR}/websocket/wsserver_api.c
)
ameba_target_include(src_${d_MCU_PROJECT_NAME}_network_websocket p_SCOPE interface
    ${c_CMPT_NETWORK_DIR}/websocket
)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_network_xip p_SCOPE interface
    ${c_CMPT_NETWORK_DIR}/sntp/sntp.c
    ${c_CMPT_NETWORK_DIR}/websocket/wsserver_tls.c
    ${c_CMPT_NETWORK_DIR}/websocket/wsclient_tls.c
    ${c_CMPT_NETWORK_DIR}/coap/sn_coap_ameba_port.c
    ${c_CMPT_NETWORK_DIR}/coap/sn_coap_builder.c
    ${c_CMPT_NETWORK_DIR}/coap/sn_coap_header_check.c
    ${c_CMPT_NETWORK_DIR}/coap/sn_coap_parser.c
    ${c_CMPT_NETWORK_DIR}/coap/sn_coap_protocol.c
    ${c_LWIP_DIR}/src/core/ipv4/dhcp.c
    ${c_LWIP_DIR}/src/core/ipv6/dhcp6.c
    ${c_LWIP_DIR}/src/core/dns.c
    ${c_CMPT_NETWORK_DIR}/dhcp/dhcps.c
    ${c_LWIP_DIR}/src/core/init.c
    ${c_LWIP_DIR}/src/core/ipv4/autoip.c
    ${c_LWIP_DIR}/src/core/ipv4/icmp.c
    ${c_LWIP_DIR}/src/core/ipv6/icmp6.c
    ${c_LWIP_DIR}/src/core/ipv6/inet6.c
    ${c_LWIP_DIR}/src/core/ipv6/mld6.c
    ${c_LWIP_DIR}/src/core/ipv6/nd6.c
    ${c_LWIP_DIR}/src/api/err.c
    ${c_CMPT_NETWORK_DIR}/httpc/httpc_tls.c
    ${c_CMPT_NETWORK_DIR}/httpd/httpd_tls.c
    ${c_CMPT_NETWORK_DIR}/mDNS/mDNSPlatform.c
    ${c_LWIP_DIR}/src/core/stats.c
    ${c_LWIP_DIR}/src/core/sys.c
    ${c_CMPT_LWIP_DIR}/api/lwip_netconf.c
)
#--------------------------#