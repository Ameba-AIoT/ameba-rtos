set(DIR ${BASEDIR}/component/network/websocket)
set(MODULE_IFLAGS ${BASEDIR}/component/network/websocket)

set(
    CSRC 
    ${DIR}/libwsclient.c 
    ${DIR}/ws_server_msg.c 
    ${DIR}/wsclient_api.c 
    ${DIR}/wsserver_api.c 
)


ameba_app_library(websocket)
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})