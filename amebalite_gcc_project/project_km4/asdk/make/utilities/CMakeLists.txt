set(DIRNET  ${BASEDIR}/component/network)
set(DIRAPP  ${BASEDIR}/component/network/iperf3)

set(
    MODULE_IFLAGS 
    ${BASEDIR}/component/network/httplite
    ${BASEDIR}/component/network/xml
    ${BASEDIR}/component/network/cJSON
    ${BASEDIR}/component/network/iperf3
)

set(
    CSRC 
    ${DIRNET}/iperf/iperf.c 
	${DIRNET}/ping/ping_test.c 
	${DIRNET}/httplite/http_client.c 
	${DIRNET}/xml/xml.c 
	${DIRNET}/cJSON/cJSON.c 
    #${DIRNET}/webserver/webserver.c


    ${DIRNET}/iperf3/dscp.c 
	${DIRNET}/iperf3/iperf_api.c 
	${DIRNET}/iperf3/iperf_auth.c 
	${DIRNET}/iperf3/iperf_client_api.c 
	${DIRNET}/iperf3/iperf_error.c 
	${DIRNET}/iperf3/iperf_locale.c 
	${DIRNET}/iperf3/iperf_sctp.c 
	${DIRNET}/iperf3/iperf_server_api.c 
	${DIRNET}/iperf3/iperf_tcp.c 
	${DIRNET}/iperf3/iperf_udp.c 
	${DIRNET}/iperf3/iperf_util.c 
	${DIRNET}/iperf3/net.c 
	${DIRNET}/iperf3/tcp_info.c 
	${DIRNET}/iperf3/tcp_window_size.c 
	${DIRNET}/iperf3/timer.c 
	${DIRNET}/iperf3/units.c 
	${DIRNET}/iperf3/iperf3.c 
	#${DIRNET}/iperf3/t_uuid.c 
	#${DIRNET}/iperf3/t_timer.c 
	#${DIRNET}/iperf3/t_units.c
)


ameba_internal_library(utilities)
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
