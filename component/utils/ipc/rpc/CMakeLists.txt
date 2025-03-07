

ameba_add_subdirectory(hal)
ameba_add_subdirectory(src/aidl)
ameba_add_subdirectory_if(CONFIG_RPC_TEST tests)
ameba_add_subdirectory(src)