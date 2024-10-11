set(DIR ${BASEDIR}/component/bluetooth)

set(
    CSRC
    ${DIR}/driver/hci/hci_process/hci_process.c 
	${DIR}/driver/hci/hci_process/hci_common.c 
	${DIR}/driver/hci/hci_process/hci_standalone.c 
	${DIR}/driver/hci/hci_transport/hci_h4.c 
	${DIR}/driver/platform/amebadplus/hci/bt_normal_patch.c 
	${DIR}/driver/platform/amebadplus/hci/bt_mp_patch.c 
	${DIR}/driver/platform/amebadplus/hci/hci_platform.c 
	${DIR}/driver/platform/amebadplus/hci/hci_uart.c 
	${DIR}/driver/dtm/hci_dtm.c 
	${DIR}/osif/bt_debug.c 
	${DIR}/osif/osif.c
	${DIR}/bt_mp/bt_mp_api.c 
	${DIR}/bt_mp/bt_fast_mp_common.c 
	${DIR}/bt_mp/bt_fast_mp_dplus.c
)

if(NOT CONFIG_BT_INIC)
    if(CONFIG_BT_ZEPHYR)
        list(
            APPEND CSRC 
            ${DIR}/driver/hci/hci_if_zephyr.c
        )
    else()
        list(
            APPEND CSRC 
            ${DIR}/driver/hci/hci_if_rtk.c 
            ${DIR}/rtk_stack/platform/common/platform_utils.c 
            ${DIR}/rtk_stack/platform/common/trace_uart.c
        )
    endif()
endif()

target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
