if (__INCLUDED_UTILITIES_BUILD_CMAKE__)
    return()
else()
    set(__INCLUDED_UTILITIES_BUILD_CMAKE__ TRUE)
endif()

add_library(src_${d_MCU_PROJECT_NAME}_utilities INTERFACE)

#--------------------------#
if(${d_PLATFORM_TYPE} STREQUAL "amebasmart")
    ameba_target_sources(src_${d_MCU_PROJECT_NAME}_utilities p_SCOPE interface
        ${c_CMPT_NETWORK_DIR}/iperf/iperf.c
        ${c_CMPT_NETWORK_DIR}/ping/ping_test.c
        ${c_CMPT_NETWORK_DIR}/httplite/http_client.c
        ${c_CMPT_NETWORK_DIR}/xml/xml.c
        ${c_CMPT_NETWORK_DIR}/cJSON/cJSON.c
        # ${c_CMPT_NETWORK_DIR}/webserver/webserver.c
    )
    if("${d_MCU_PROJECT_NAME}" STREQUAL "ap")
        ameba_target_sources(src_${d_MCU_PROJECT_NAME}_utilities p_SCOPE interface
            ${c_CMPT_NETWORK_DIR}/iperf3/dscp.c
            ${c_CMPT_NETWORK_DIR}/iperf3/iperf_api.c
            ${c_CMPT_NETWORK_DIR}/iperf3/iperf_auth.c
            ${c_CMPT_NETWORK_DIR}/iperf3/iperf_client_api.c
            ${c_CMPT_NETWORK_DIR}/iperf3/iperf_error.c
            ${c_CMPT_NETWORK_DIR}/iperf3/iperf_locale.c
            ${c_CMPT_NETWORK_DIR}/iperf3/iperf_sctp.c
            ${c_CMPT_NETWORK_DIR}/iperf3/iperf_server_api.c
            ${c_CMPT_NETWORK_DIR}/iperf3/iperf_tcp.c
            ${c_CMPT_NETWORK_DIR}/iperf3/iperf_udp.c
            ${c_CMPT_NETWORK_DIR}/iperf3/iperf_util.c
            ${c_CMPT_NETWORK_DIR}/iperf3/net.c
            ${c_CMPT_NETWORK_DIR}/iperf3/tcp_info.c
            ${c_CMPT_NETWORK_DIR}/iperf3/tcp_window_size.c
            ${c_CMPT_NETWORK_DIR}/iperf3/timer.c
            ${c_CMPT_NETWORK_DIR}/iperf3/units.c
            ${c_CMPT_NETWORK_DIR}/iperf3/iperf3.c

            # ${c_CMPT_NETWORK_DIR}/iperf3/t_uuid.c
            # ${c_CMPT_NETWORK_DIR}/iperf3/t_timer.c
            # ${c_CMPT_NETWORK_DIR}/iperf3/t_units.c
        )
        ameba_target_include(src_${d_MCU_PROJECT_NAME}_utilities p_SCOPE interface
            ${c_CMPT_NETWORK_DIR}/iperf3 #WARNING: this folder contains queue.h, may case proble with freertos's queue.h
        )
    endif()
endif()
#--------------------------#