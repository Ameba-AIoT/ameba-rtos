ameba_add_subdirectory(ringbuffer)

if (CONFIG_WHC_HOST OR CONFIG_WHC_NONE)
    ameba_add_subdirectory_if(CONFIG_UNITY_TEST unity)
    ameba_add_subdirectory_if(CONFIG_UNITY_TEST unity/cases)
endif()

ameba_add_subdirectory_if(CONFIG_IMQ_EN ipc/ipc)
ameba_add_subdirectory_if(CONFIG_RPC_EN ipc/rpc)
