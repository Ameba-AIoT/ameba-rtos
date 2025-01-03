set(BLEDIR ${BASEDIR}/component/bluetooth)
list(
    APPEND GLOBAL_IFLAGS
	${BASEDIR}/component/soc/amebalite/fwlib/include
	${BLEDIR}/rtk_coex
	${BLEDIR}/driver
	${BLEDIR}/driver/hci
	${BLEDIR}/driver/inc
	${BLEDIR}/driver/inc/hci
	${BLEDIR}/driver/platform/amebalite/inc
	${BLEDIR}/osif
	${BLEDIR}/bt_audio/include
	${BLEDIR}/bt_audio/bt_codec
	${BLEDIR}/bt_mp
)

if(CONFIG_BT_ZEPHYR)
    list(
        APPEND GLOBAL_IFLAGS
        ${BLEDIR}/api/zephyr_stack
        ${BLEDIR}/zephyr
        ${BLEDIR}/zephyr/hci_driver
        ${BLEDIR}/zephyr/include
        ${BLEDIR}/zephyr/stack
        ${BLEDIR}/zephyr/stack/host
        ${BLEDIR}/zephyr/settings
        ${BLEDIR}/zephyr/port
    )
else()
    list(
        APPEND GLOBAL_IFLAGS
        ${BLEDIR}/api/rtk_stack
        ${BLEDIR}/rtk_stack/example
        ${BLEDIR}/rtk_stack/inc/app
        ${BLEDIR}/rtk_stack/inc/os
        ${BLEDIR}/rtk_stack/inc/platform
        ${BLEDIR}/rtk_stack/inc/stack
        ${BLEDIR}/rtk_stack/inc/bluetooth/gap
        ${BLEDIR}/rtk_stack/inc/bluetooth/profile
        ${BLEDIR}/rtk_stack/inc/bluetooth/profile/client
        ${BLEDIR}/rtk_stack/inc/bluetooth/profile/server
        ${BLEDIR}/rtk_stack/inc/bluetooth/leaudio
        ${BLEDIR}/rtk_stack/inc/ble_mgr
        ${BLEDIR}/rtk_stack/inc/framework/bt_mgr_inc
        ${BLEDIR}/rtk_stack/inc/framework/sysm_inc
        ${BLEDIR}/rtk_stack/platform/common
        ${BLEDIR}/rtk_stack/src/mesh/cmd
        ${BLEDIR}/rtk_stack/src/mesh/common
        ${BLEDIR}/rtk_stack/src/mesh/gap
        ${BLEDIR}/rtk_stack/src/mesh/inc
        ${BLEDIR}/rtk_stack/src/mesh/inc/amebalite
        ${BLEDIR}/rtk_stack/src/mesh/model
        ${BLEDIR}/rtk_stack/src/mesh/model/ali
        ${BLEDIR}/rtk_stack/src/mesh/model/realtek
        ${BLEDIR}/rtk_stack/src/mesh/platform
        ${BLEDIR}/rtk_stack/src/mesh/profile
        ${BLEDIR}/rtk_stack/src/mesh/utility
    )

    if(CONFIG_BT_BLE_ONLY)
        list(APPEND GLOBAL_IFLAGS ${BLEDIR}/rtk_stack/platform/amebalite/lib/km4/ble_only)
    elseif(CONFIG_BT_DUAL_MODE)
        list(APPEND GLOBAL_IFLAGS ${BLEDIR}/rtk_stack/platform/amebalite/lib/km4/dual_mode)
    endif()

endif()

list(
    APPEND GLOBAL_IFLAGS
    ${BLEDIR}/api
    ${BLEDIR}/api/include
    ${BLEDIR}/example/ble_mesh_ota_common
    ${BLEDIR}/example/bt_atcmd
    ${BLEDIR}/example/gatt_service/server
    ${BLEDIR}/example/gatt_service/client
    ${BASEDIR}/component/at_cmd
)
