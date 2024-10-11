ameba_internal_library(bluetooth)

add_subdirectory(amebadplus_bsp)
add_subdirectory(bt_coex)

if(CONFIG_BT_INIC)
    add_subdirectory(bt_inic)
else()
    add_subdirectory(bt_api)
    add_subdirectory(bt_atcmd)
endif()

if(CONFIG_BT_ZEPHYR)
    add_subdirectory(bt_zephyr)
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

if(CONFIG_BT_MESH_PROVISIONER_SUPPORT OR CONFIG_BT_MESH_DEVICE_SUPPORT)
    if(NOT CONFIG_BT_ZEPHYR)
        add_subdirectory(ble_mesh_stack)
    endif()
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

if(CONFIG_BT_TRANSFER_MODULE)
    add_subdirectory(ble_transfer_module)
endif()