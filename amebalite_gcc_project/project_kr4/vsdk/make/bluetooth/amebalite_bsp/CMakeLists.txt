set(DIR ${BASEDIR}/component/bluetooth)

set(
    MODULE_IFLAGS
    ${BASEDIR}/component/wifi/driver
	${BASEDIR}/component/wifi/driver/phl
	${BASEDIR}/component/wifi/driver/phl_g6
	${BASEDIR}/component/wifi/driver/phldm
	${BASEDIR}/component/wifi/driver/halmac
	${BASEDIR}/component/wifi/driver/halcommon
	${BASEDIR}/component/wifi/driver/halbtc
	${BASEDIR}/component/wifi/driver/rtl8720e
	${BASEDIR}/component/wifi/driver/rtl8720e_rom
)

set(
    CSRC
    ${DIR}/driver/hci/hci_process/hci_process.c
	${DIR}/driver/hci/hci_process/hci_download_v2.c
	${DIR}/driver/hci/hci_process/hci_download_v3.c
	${DIR}/driver/hci/hci_process/hci_common.c
	${DIR}/driver/hci/hci_process/hci_standalone.c
	${DIR}/driver/hci/hci_transport/hci_h4.c
	${DIR}/driver/platform/amebalite/hci/bt_normal_patch.c
	${DIR}/driver/platform/amebalite/hci/bt_mp_patch.c
	${DIR}/driver/platform/amebalite/hci/hci_platform.c
	${DIR}/driver/platform/amebalite/hci/hci_uart.c
	${DIR}/osif/bt_debug.c
	${DIR}/osif/osif.c
	${DIR}/bt_mp/bt_mp_api.c
)

if(CONFIG_BT_ZEPHYR)
    list(APPEND CSRC ${DIR}/driver/hci/hci_if_zephyr.c)
else()
    list(
        APPEND CSRC
        ${DIR}/driver/hci/hci_if_rtk.c
        ${DIR}/rtk_stack/platform/common/platform_utils.c
        ${DIR}/rtk_stack/platform/common/trace_uart.c
    )
endif()

target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
