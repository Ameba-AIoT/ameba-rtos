ameba_add_subdirectory(ringbuffer)

ameba_add_subdirectory_if(CONFIG_IMQ_EN ipc/ipc)
ameba_add_subdirectory_if(CONFIG_RPC_EN ipc/rpc)
