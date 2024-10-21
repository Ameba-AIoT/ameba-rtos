set(LWIPDIR ${BASEDIR}/component/lwip/lwip_${LWIP_VER})
set(LWIPAPI ${BASEDIR}/component/lwip/api)
set(NETWORK ${BASEDIR}/component/network)
set(LWIPPORT ${LWIPDIR}/port/realtek/freertos)
set(MODULE_IFLAGS)

if(CONFIG_802154_THREAD_BORDER_ROUTER_EN)
    list(APPEND MODULE_IFLAGS ${BASEDIR}/component/wpan/openthread/rtk_otbr)
endif()

set(
    CSRC
    ${NETWORK}/sntp/sntp.c
    ${NETWORK}/websocket/wsserver_tls.c
    ${NETWORK}/websocket/wsclient_tls.c
    ${NETWORK}/dhcp/dhcps.c
    ${NETWORK}/httpc/httpc_tls.c
    ${NETWORK}/httpd/httpd_tls.c
    ${NETWORK}/mDNS/mDNSPlatform.c
    #${NETWORK}/coap/sn_coap_ameba_port.c
    #${NETWORK}/coap/sn_coap_builder.c
    #${NETWORK}/coap/sn_coap_header_check.c
    #${NETWORK}/coap/sn_coap_parser.c
    #${NETWORK}/coap/sn_coap_protocol.c

    ${LWIPDIR}/src/core/ipv4/dhcp.c
    ${LWIPDIR}/src/core/ipv6/dhcp6.c
    ${LWIPDIR}/src/core/dns.c
    ${LWIPDIR}/src/core/init.c
    ${LWIPDIR}/src/core/ipv4/autoip.c
    ${LWIPDIR}/src/core/ipv4/icmp.c
    ${LWIPDIR}/src/core/ipv6/icmp6.c
    ${LWIPDIR}/src/core/ipv6/inet6.c
    ${LWIPDIR}/src/core/ipv6/mld6.c
    ${LWIPDIR}/src/core/ipv6/nd6.c
    ${LWIPDIR}/src/api/err.c
    ${LWIPDIR}/src/core/stats.c
    ${LWIPDIR}/src/core/sys.c
    ${LWIPDIR}/src/core/tcp_in.c
    ${LWIPDIR}/src/core/tcp_out.c
    ${LWIPDIR}/src/core/def.c
    ${LWIPDIR}/src/core/ipv4/igmp.c
    ${LWIPDIR}/src/core/pbuf.c
    ${LWIPDIR}/src/core/inet_chksum.c
    ${LWIPDIR}/src/api/api_msg.c
    ${LWIPDIR}/src/api/netbuf.c
    ${LWIPDIR}/port/realtek/freertos/sys_arch.c
    ${LWIPDIR}/port/realtek/freertos/ethernetif.c
    ${LWIPDIR}/src/core/timeouts.c
    ${LWIPDIR}/src/core/raw.c
    ${LWIPDIR}/src/core/mem.c
    ${LWIPDIR}/src/core/memp.c
    ${LWIPDIR}/src/core/netif.c
    ${LWIPDIR}/src/core/ip.c
    ${LWIPDIR}/src/core/ipv4/ip4.c
    ${LWIPDIR}/src/core/ipv4/ip4_addr.c
    ${LWIPDIR}/src/core/ipv4/ip4_frag.c
    ${LWIPDIR}/src/core/ipv6/ip6.c
    ${LWIPDIR}/src/core/ipv6/ip6_addr.c
    ${LWIPDIR}/src/core/ipv6/ip6_frag.c
    ${LWIPDIR}/src/core/tcp.c
    ${LWIPDIR}/src/core/udp.c
    ${LWIPDIR}/src/api/sockets.c
    ${LWIPDIR}/src/api/tcpip.c
    ${LWIPDIR}/src/netif/ethernet.c
    ${LWIPDIR}/src/core/ipv4/etharp.c
    ${LWIPDIR}/src/core/ipv6/ethip6.c
    ${LWIPDIR}/src/api/netdb.c
    ${LWIPDIR}/src/api/api_lib.c
    ${LWIPDIR}/src/api/netifapi.c
    ${LWIPDIR}/src/apps/mdns/mdns.c

    ${LWIPAPI}/lwip_netconf.c
)

ameba_internal_library(lwip)
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
