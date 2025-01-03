set(BLEDIR ${BASEDIR}/component/bluetooth)
list(
    APPEND GLOBAL_IFLAGS
    ${BASEDIR}/component/soc/amebadplus/fwlib/include
    ${BLEDIR}/rtk_coex
    ${BLEDIR}/driver
    ${BLEDIR}/driver/dtm
    ${BLEDIR}/driver/hci
    ${BLEDIR}/driver/inc
    ${BLEDIR}/driver/inc/hci
    ${BLEDIR}/driver/platform/amebadplus/inc
    ${BLEDIR}/osif
    ${BLEDIR}/bt_mp
)
list(
    APPEND GLOBAL_IFLAGS
    ${BLEDIR}/api
    ${BLEDIR}/api/include
    ${BLEDIR}/example/bt_atcmd
    ${BLEDIR}/example/gatt_service/server
    ${BLEDIR}/example/gatt_service/client
    ${BASEDIR}/component/at_cmd
)
if(CONFIG_BT_INIC)
    list(
        APPEND GLOBAL_IFLAGS
        ${BLEDIR}/driver/bt_inic
    )
endif()