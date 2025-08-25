ameba_add_subdirectory(bt_atcmd)

ameba_add_subdirectory(gatt_service/client)
ameba_add_subdirectory(gatt_service/server)

ameba_add_subdirectory_if(CONFIG_BT_LE_AUDIO
    ble_audio
)

ameba_add_subdirectory_if(CONFIG_BT_A2DP_LE_AUDIO_PBP
    bt_a2dp_pbp
)

ameba_add_subdirectory_if(CONFIG_BT_A2DP_HFP_LE_AUDIO_PBP
    bt_a2dp_hfp_pbp
)

ameba_add_subdirectory_if(CONFIG_BT_A2DP_LE_AUDIO_TMAP
    bt_a2dp_tmap
)

ameba_add_subdirectory_if(CONFIG_BT_CENTRAL
    ble_central
)

ameba_add_subdirectory_if(CONFIG_BT_ISO_TEST
    ble_iso
)

ameba_add_subdirectory_if(CONFIG_BT_HOGP
    ble_hogp_gamepad
)

ameba_add_subdirectory_if(CONFIG_BT_MESH_DEVICE
    ble_mesh_device
)

ameba_add_subdirectory_if(CONFIG_BT_MESH_DEVICE_SCATTERNET
    ble_mesh_device_scatternet
)

ameba_add_subdirectory_if(CONFIG_BT_MESH_DEVICE_TEST
    ble_mesh_device_test
)

if(CONFIG_BT_MESH_PROVISIONER_SUPPORT OR CONFIG_BT_MESH_DEVICE_SUPPORT)
    ameba_add_subdirectory(
        ble_mesh_ota
    )
endif()

ameba_add_subdirectory_if(CONFIG_BT_MESH_PROVISIONER
    ble_mesh_provisioner
)

ameba_add_subdirectory_if(CONFIG_BT_MESH_PROVISIONER_SCATTERNET
    ble_mesh_provisioner_scatternet
)

ameba_add_subdirectory_if(CONFIG_BT_MESH_PROVISIONER_TEST
    ble_mesh_provisioner_test
)

ameba_add_subdirectory_if(CONFIG_BT_PERIPHERAL
    ble_peripheral
)

ameba_add_subdirectory_if(CONFIG_BT_SCATTERNET
    ble_scatternet
)

ameba_add_subdirectory_if(CONFIG_BT_THROUGHPUT
    ble_throughput
)

ameba_add_subdirectory_if(CONFIG_BT_TRANSFER_MODULE
    ble_transfer_module
)

ameba_add_subdirectory_if(CONFIG_BT_A2DP
    bt_a2dp
)

ameba_add_subdirectory_if(CONFIG_BT_A2DP_HFP
    bt_a2dp_hfp
)

ameba_add_subdirectory_if(CONFIG_BT_A2DP_PROVISIONER_SCATTERNET
	bt_a2dp_provisioner_scatternet
)

ameba_add_subdirectory_if(CONFIG_BT_A2DP_SCATTERNET
    bt_a2dp_scatternet
)

ameba_add_subdirectory_if(CONFIG_BT_APP_AUDIO_DATA
    bt_app_audio_data
)

ameba_add_subdirectory_if(CONFIG_BT_AUDIO_MP_TEST
    bt_audio_mp_test
)

ameba_add_subdirectory_if(CONFIG_BT_HFP
    bt_hfp
)

ameba_add_subdirectory_if(CONFIG_BT_HID
    bt_hid
)

ameba_add_subdirectory_if(CONFIG_WHC_FULLMAC_HOST
    bt_host/rtos_driver
)

ameba_add_subdirectory_if(CONFIG_BT_PTS
    bt_pts
)

ameba_add_subdirectory_if(CONFIG_BT_RFC
    bt_rfc
)
ameba_add_subdirectory_if(CONFIG_BT_SPP
    bt_spp
)

if(CONFIG_BT_WIFIMATE_DEVICE OR CONFIG_BT_WIFIMATE_CONFIGURATOR)
    ameba_add_subdirectory(
        ble_wifimate
    )
endif()

if (CONFIG_BT_OTA_CENTRAL OR CONFIG_BT_OTA_PERIPHERAL)
    ameba_add_subdirectory(
        ble_ota
    )
endif()