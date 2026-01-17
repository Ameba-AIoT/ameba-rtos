##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin

# You may use if-else condition to set or update predefined variable above
ameba_list_append(public_includes
    lwip_${v_LWIP_VER}/src/include
    lwip_${v_LWIP_VER}/src/include/lwip/apps
    lwip_${v_LWIP_VER}/src/include/netif
    lwip_${v_LWIP_VER}/port/realtek
    lwip_${v_LWIP_VER}/port/realtek/freertos
    api
)

# Component public part, user config end
#----------------------------------------#

#WARNING: Fixed section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_definitions})
ameba_global_library(${public_libraries}) #default: whole-archived

if(NOT (CONFIG_LWIP_LAYER))
    return()
endif()

##########################################################################################
## * This part defines private part of the component
## * Private part is used to build target of current component
## * NOTE: The build API guarantees the global build configures(mentioned above)
## *       applied to the target automatically. So if any configure was already added
## *       to public above, it's unnecessary to add again below.

#NOTE: User defined section, add your private build configures here
# You may use if-else condition to set these predefined variable
# They are only for ameba_add_internal_library/ameba_add_external_app_library/ameba_add_external_soc_library
set(private_sources)                 #private source files, NOTE: relative path is OK
set(private_includes)                #private include directories, NOTE: relative path is OK
set(private_definitions)             #private definitions
set(private_compile_options)         #private compile_options

#------------------------------#
# Component private part, user config begin
ameba_list_append(private_sources
    lwip_${v_LWIP_VER}/src/core/ipv4/dhcp.c
    lwip_${v_LWIP_VER}/src/core/ipv6/dhcp6.c
    lwip_${v_LWIP_VER}/src/core/dns.c
    lwip_${v_LWIP_VER}/src/core/init.c
    lwip_${v_LWIP_VER}/src/core/ipv4/autoip.c
    lwip_${v_LWIP_VER}/src/core/ipv4/icmp.c
    lwip_${v_LWIP_VER}/src/core/ipv6/icmp6.c
    lwip_${v_LWIP_VER}/src/core/ipv6/inet6.c
    lwip_${v_LWIP_VER}/src/core/ipv6/mld6.c
    lwip_${v_LWIP_VER}/src/core/ipv6/nd6.c
    lwip_${v_LWIP_VER}/src/api/err.c
    lwip_${v_LWIP_VER}/src/core/stats.c
    lwip_${v_LWIP_VER}/src/core/sys.c
    api/lwip_netconf.c

    lwip_${v_LWIP_VER}/src/core/tcp_in.c
    lwip_${v_LWIP_VER}/src/core/tcp_out.c
    lwip_${v_LWIP_VER}/src/core/def.c
    lwip_${v_LWIP_VER}/src/core/ipv4/igmp.c
    lwip_${v_LWIP_VER}/src/core/pbuf.c
    lwip_${v_LWIP_VER}/src/core/inet_chksum.c
    lwip_${v_LWIP_VER}/src/api/api_msg.c
    lwip_${v_LWIP_VER}/src/api/netbuf.c
    lwip_${v_LWIP_VER}/port/realtek/freertos/sys_arch.c
    lwip_${v_LWIP_VER}/port/realtek/freertos/ethernetif.c
    lwip_${v_LWIP_VER}/src/core/timeouts.c
    lwip_${v_LWIP_VER}/src/core/raw.c
    lwip_${v_LWIP_VER}/src/core/mem.c
    lwip_${v_LWIP_VER}/src/core/memp.c
    lwip_${v_LWIP_VER}/src/core/netif.c
    lwip_${v_LWIP_VER}/src/core/ip.c
    lwip_${v_LWIP_VER}/src/core/ipv4/ip4.c
    lwip_${v_LWIP_VER}/src/core/ipv4/ip4_addr.c
    lwip_${v_LWIP_VER}/src/core/ipv4/ip4_frag.c
    lwip_${v_LWIP_VER}/src/core/ipv6/ip6.c
    lwip_${v_LWIP_VER}/src/core/ipv6/ip6_addr.c
    lwip_${v_LWIP_VER}/src/core/ipv6/ip6_frag.c
    lwip_${v_LWIP_VER}/src/core/tcp.c
    lwip_${v_LWIP_VER}/src/core/udp.c
    lwip_${v_LWIP_VER}/src/api/sockets.c
    lwip_${v_LWIP_VER}/src/api/tcpip.c
    lwip_${v_LWIP_VER}/src/netif/ethernet.c
    lwip_${v_LWIP_VER}/src/core/ipv4/etharp.c
    lwip_${v_LWIP_VER}/src/core/ipv6/ethip6.c
    lwip_${v_LWIP_VER}/src/api/netdb.c
    lwip_${v_LWIP_VER}/src/api/api_lib.c
    lwip_${v_LWIP_VER}/src/api/netifapi.c
    lwip_${v_LWIP_VER}/src/apps/mdns/mdns.c
    lwip_${v_LWIP_VER}/src/apps/sntp/sntp.c
)

#WARNING: For toolchain version 12.3.1, code like this will cause a warning:
#  ip_addr_isany(&cpcb->local_ip)
ameba_list_append(private_compile_options
    -Wno-address
)

# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

###NOTE: For open-source component, always build from source
ameba_add_internal_library(lwip
    p_SOURCES
        ${private_sources}
    p_INCLUDES
        ${private_includes}
    p_DEFINITIONS
        ${private_definitions}
    p_COMPILE_OPTIONS
        ${private_compile_options}
)
if (CONFIG_AMEBAGREEN2)
ameba_library_section_prefix(${c_CURRENT_TARGET_NAME} .lwip.cache.efficient) #fix lwip section to cache efficient area
endif()
##########################################################################################
ameba_add_subdirectory(lwip_${v_LWIP_VER}/src/core/ipv4/ip_nat)
ameba_add_subdirectory(lwip_${v_LWIP_VER}/src/core/ipv6/ip6_rlocal)
