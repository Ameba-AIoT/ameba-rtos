if(CONFIG_IMQ_EN)
	add_subdirectory(ipc)
endif()

if(CONFIG_RPC_EN)
    add_subdirectory(rpc)
endif()
