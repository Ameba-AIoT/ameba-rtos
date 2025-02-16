set(DIR ${BASEDIR}/component/wifi/wifi_tunnel_app)

set(CSRC)

if(CONFIG_AS_INIC_AP)
    list(APPEND CSRC ${DIR}/wtn_app_socket.c)
    list(APPEND CSRC ${DIR}/wtn_app_rnat.c)
endif()

if (CSRC)
ameba_internal_library(wifi_tunnel_app)
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
endif()