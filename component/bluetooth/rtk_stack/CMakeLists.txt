ameba_add_subdirectory(platform)

if(CONFIG_BT_MESH_PROVISIONER_SUPPORT OR CONFIG_BT_MESH_DEVICE_SUPPORT)
	ameba_add_subdirectory(src/mesh)
endif()
