ameba_internal_library(bluetooth)

add_subdirectory(amebalite_bsp)
add_subdirectory(bt_coex)
add_subdirectory(bt_api)
add_subdirectory(bt_atcmd)

if(CONFIG_BT_ZEPHYR)
	add_subdirectory(bt_zephyr)
endif()

if(CONFIG_BT_MESH_PROVISIONER_SUPPORT OR CONFIG_BT_MESH_DEVICE_SUPPORT)
    if (NOT CONFIG_BT_ZEPHYR)
        add_subdirectory(ble_mesh_stack)
    endif()
    add_subdirectory(ble_mesh_ota_common)
endif()

if(CONFIG_BT_PERIPHERAL)
	add_subdirectory(ble_peripheral)
endif()

if(CONFIG_BT_CENTRAL)
	add_subdirectory(ble_central)
endif()

if(CONFIG_BT_SCATTERNET)
	add_subdirectory(ble_scatternet)
endif()

if(CONFIG_BT_THROUGHPUT)
	add_subdirectory(ble_throughput)
endif()

if(CONFIG_BT_MESH_PROVISIONER)
	add_subdirectory(ble_mesh_provisioner)
endif()

if(CONFIG_BT_MESH_DEVICE)
	add_subdirectory(ble_mesh_device)
endif()

if(CONFIG_BT_MESH_PROVISIONER_SCATTERNET)
	add_subdirectory(ble_mesh_provisioner_scatternet)
endif()

if(CONFIG_BT_MESH_DEVICE_SCATTERNET)
	add_subdirectory(ble_mesh_device_scatternet)
endif()

if(CONFIG_BT_MESH_PROVISIONER_TEST)
	add_subdirectory(ble_mesh_provisioner_test)
endif()

if(CONFIG_BT_MESH_DEVICE_TEST)
	add_subdirectory(ble_mesh_device_test)
endif()

if(CONFIG_BT_LE_AUDIO)
	add_subdirectory(ble_audio)
endif()

if(CONFIG_BT_ISO_TEST)
	add_subdirectory(ble_iso)
endif()

if(CONFIG_BT_APP_AUDIO_DATA)
	add_subdirectory(bt_app_audio_data)
endif()

if(CONFIG_BT_A2DP)
	add_subdirectory(bt_a2dp)
endif()

if(CONFIG_BT_A2DP_PROVISIONER_SCATTERNET)
	add_subdirectory(bt_a2dp_provisioner_scatternet)
endif()

if(CONFIG_BT_A2DP_HFP)
	add_subdirectory(bt_a2dp_hfp)
endif()

if(CONFIG_BT_SPP)
	add_subdirectory(bt_spp)
endif()

if(CONFIG_BT_RFC)
	add_subdirectory(bt_rfc)
endif()

if(CONFIG_BT_HID)
	add_subdirectory(bt_hid)
endif()

if(CONFIG_BT_HFP)
	add_subdirectory(bt_hfp)
endif()

if(CONFIG_BT_A2DP_SCATTERNET)
	add_subdirectory(bt_a2dp_scatternet)
endif()

if(CONFIG_BT_AUDIO_MP_TEST)
	add_subdirectory(bt_audio_mp_test)
endif()

if(CONFIG_BT_A2DP_LE_AUDIO_PBP)
	add_subdirectory(bt_a2dp_pbp)
endif()

if(CONFIG_BT_A2DP_LE_AUDIO_TMAP)
	add_subdirectory(bt_a2dp_tmap)
endif()

if(CONFIG_BT_PTS)
	add_subdirectory(bt_pts)
endif()

if(CONFIG_BT_LE_AUDIO OR CONFIG_BT_A2DP OR CONFIG_BT_A2DP_PROVISIONER_SCATTERNET OR
CONFIG_BT_A2DP_HFP OR CONFIG_BT_HFP OR CONFIG_BT_A2DP_SCATTERNET OR
CONFIG_BT_AUDIO_MP_TEST OR CONFIG_BT_A2DP_LE_AUDIO_PBP OR CONFIG_BT_A2DP_LE_AUDIO_TMAP)
	add_subdirectory(bt_audio)
endif()