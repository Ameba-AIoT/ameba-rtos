set(_tmp_${d_MCU_PROJECT_NAME}_bluetooth_subdirectories)

ameba_list_append(_tmp_${d_MCU_PROJECT_NAME}_bluetooth_subdirectories
    amebasmart_bsp
    bt_coex
)

ameba_list_append_if(CONFIG_BT_INIC _tmp_${d_MCU_PROJECT_NAME}_bluetooth_subdirectories
    bt_inic
    p_ELSE
    bt_api
    bt_atcmd
)

ameba_list_append_if(CONFIG_BT_ZEPHYR _tmp_${d_MCU_PROJECT_NAME}_bluetooth_subdirectories
    bt_zephyr
)
if(CONFIG_BT_MESH_PROVISIONER_SUPPORT OR CONFIG_BT_MESH_DEVICE_SUPPORT)
    ameba_list_append_ifnot(CONFIG_BT_ZEPHYR _tmp_${d_MCU_PROJECT_NAME}_bluetooth_subdirectories
        ble_mesh_stack
    )
    ameba_list_append(_tmp_${d_MCU_PROJECT_NAME}_bluetooth_subdirectories
        ble_mesh_ota_common
    )
endif()

ameba_list_append_if(CONFIG_BT_PERIPHERAL _tmp_${d_MCU_PROJECT_NAME}_bluetooth_subdirectories
    ble_peripheral
)

ameba_list_append_if(CONFIG_BT_CENTRAL _tmp_${d_MCU_PROJECT_NAME}_bluetooth_subdirectories
    ble_central
)

ameba_list_append_if(CONFIG_BT_SCATTERNET _tmp_${d_MCU_PROJECT_NAME}_bluetooth_subdirectories
    ble_scatternet
)

ameba_list_append_if(CONFIG_BT_THROUGHPUT _tmp_${d_MCU_PROJECT_NAME}_bluetooth_subdirectories
    ble_throughput
)

ameba_list_append_if(CONFIG_BT_MESH_PROVISIONER _tmp_${d_MCU_PROJECT_NAME}_bluetooth_subdirectories
    ble_mesh_provisioner
)

ameba_list_append_if(CONFIG_BT_MESH_DEVICE _tmp_${d_MCU_PROJECT_NAME}_bluetooth_subdirectories
    ble_mesh_device
)

ameba_list_append_if(CONFIG_BT_MESH_PROVISIONER_SCATTERNET _tmp_${d_MCU_PROJECT_NAME}_bluetooth_subdirectories
    ble_mesh_provisioner_scatternet
)

ameba_list_append_if(CONFIG_BT_MESH_DEVICE_SCATTERNET _tmp_${d_MCU_PROJECT_NAME}_bluetooth_subdirectories
    ble_mesh_device_scatternet
)

ameba_list_append_if(CONFIG_BT_MESH_PROVISIONER_TEST _tmp_${d_MCU_PROJECT_NAME}_bluetooth_subdirectories
    ble_mesh_provisioner_test
)

ameba_list_append_if(CONFIG_BT_MESH_DEVICE_TEST _tmp_${d_MCU_PROJECT_NAME}_bluetooth_subdirectories
    ble_mesh_device_test
)

ameba_list_append_if(CONFIG_BT_LE_AUDIO _tmp_${d_MCU_PROJECT_NAME}_bluetooth_subdirectories
    ble_audio_common
    ble_audio
    bt_audio
)

ameba_list_append_if(CONFIG_BT_ISO_TEST _tmp_${d_MCU_PROJECT_NAME}_bluetooth_subdirectories
    ble_iso
)

ameba_list_append_if(CONFIG_BT_APP_AUDIO_DATA _tmp_${d_MCU_PROJECT_NAME}_bluetooth_subdirectories
    bt_app_audio_data
)

ameba_list_append_if(CONFIG_BT_A2DP _tmp_${d_MCU_PROJECT_NAME}_bluetooth_subdirectories
    bt_a2dp
    bt_audio
)

ameba_list_append_if(CONFIG_BT_A2DP_HFP _tmp_${d_MCU_PROJECT_NAME}_bluetooth_subdirectories
    bt_a2dp_hfp
    bt_audio
)

ameba_list_append_if(CONFIG_BT_SPP _tmp_${d_MCU_PROJECT_NAME}_bluetooth_subdirectories
    bt_spp
)

ameba_list_append_if(CONFIG_BT_RFC _tmp_${d_MCU_PROJECT_NAME}_bluetooth_subdirectories
    bt_rfc
)

ameba_list_append_if(CONFIG_BT_HID _tmp_${d_MCU_PROJECT_NAME}_bluetooth_subdirectories
    bt_hid
)

ameba_list_append_if(CONFIG_BT_HFP _tmp_${d_MCU_PROJECT_NAME}_bluetooth_subdirectories
    bt_hfp
    bt_audio
)

ameba_list_append_if(CONFIG_BT_A2DP_SCATTERNET _tmp_${d_MCU_PROJECT_NAME}_bluetooth_subdirectories
    bt_a2dp_scatternet
    bt_audio
)

ameba_list_append_if(CONFIG_BT_AUDIO_MP_TEST _tmp_${d_MCU_PROJECT_NAME}_bluetooth_subdirectories
    bt_audio_mp_test
    bt_audio
)

ameba_list_append_if(CONFIG_BT_A2DP_LE_AUDIO_PBP _tmp_${d_MCU_PROJECT_NAME}_bluetooth_subdirectories
    ble_audio_common
    bt_a2dp_pbp
    bt_audio
)

ameba_list_append_if(CONFIG_BT_A2DP_LE_AUDIO_TMAP _tmp_${d_MCU_PROJECT_NAME}_bluetooth_subdirectories
    ble_audio_common
    bt_a2dp_tmap
    bt_audio
)

# Remove duplicates subdirectories
list(REMOVE_DUPLICATES _tmp_${d_MCU_PROJECT_NAME}_bluetooth_subdirectories)
ameba_add_subdirectories(${_tmp_${d_MCU_PROJECT_NAME}_bluetooth_subdirectories})
unset(_tmp_${d_MCU_PROJECT_NAME}_bluetooth_subdirectories)

# Due to some subdirectories target have overlap sources(source file in component/bluetooth/example),
# compile them seperately will cause mutiple definition.
# So here build them to single one target
ameba_target_add(bluetooth_example
    p_WRAP_TARGET_NAME
    p_ADD_EMPTY_C_FILE
    p_PREFIX lib_
    p_LINK_LIBRARY
        d_PROJ_${d_MCU_PROJECT_NAME_UPPER}_INCLUDE_DIRECTORIES
        d_PROJ_${d_MCU_PROJECT_NAME_UPPER}_LINK_OPTIONS
        src_${d_MCU_PROJECT_NAME}_bluetooth_example
    p_APPEND_TO_LIST
        g_${d_MCU_PROJECT_NAME}_IMAGE2_LIBS
    p_DEPENDENTED_BY_TARGETS
        dep_${d_MCU_PROJECT_NAME}_audio
)
