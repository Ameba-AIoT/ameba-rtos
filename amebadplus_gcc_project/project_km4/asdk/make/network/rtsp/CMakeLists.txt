set(DIR ${BASEDIR}/component/network/rtsp)
set(MODULE_IFLAGS ${DIR})

set(
    CSRC
    ${DIR}/rtp_api.c
    ${DIR}/rtsp_api.c
    ${DIR}/sdp.c
)

ameba_app_library(rstp)
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})