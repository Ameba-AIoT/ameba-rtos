##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin

# You may use if-else condition to set or update predefined variable above

## Differentiated configuration based on SoC types

# Component public part, user config end
#----------------------------------------#

#WARNING: Fixed section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_definitions})
ameba_global_library(${public_libraries}) #default: whole-archived

##########################################################################################
## * This part defines private part of the component
## * Private part is used to build target of current component
## * NOTE: The build API guarantees the global build configures(mentioned above)
## *       applied to the target automatically. So if any configure was already added
## *       to public above, it's unnecessary to add again below.

#NOTE: User defined section, add your private build configures here
# You may use if-else condition to set these predefined variable
# They are only for ameba_add_internal_library/ameba_add_external_app_library/ameba_add_external_soc_library
set(private_sources)                 #private source files, NOTE: relative path is OK
set(private_includes)                #private include directories, NOTE: relative path is OK
set(private_definitions)             #private definitions
set(private_compile_options)         #private compile_options

#------------------------------#
# Component private part, user config begin

ameba_list_append(private_sources
    rpc_test.c
    test_rpc_ap.c
    test_rpc_np.c
    test_rpc_np_client1.c
    test_rpc_np_client2.c

    aidl/Test1RPC_Agent_svc.c
    aidl/Test1RPC_Agent_xdr.c
    aidl/Test1RPCBaseDS_xdr.c
    aidl/Test1RPC_System_clnt.c
    aidl/Test1RPC_System_svc.c
    aidl/Test1RPC_System_xdr.c

    aidl/Test2RPC_Agent_svc.c
    aidl/Test2RPC_Agent_xdr.c
    aidl/Test2RPCBaseDS_xdr.c
    aidl/Test2RPC_System_clnt.c
    aidl/Test2RPC_System_svc.c
    aidl/Test2RPC_System_xdr.c
)
ameba_list_append(private_includes
    ${c_CMPT_AUDIO_DIR}/base/log/interfaces
    ${c_CMPT_AUDIO_DIR}/base/osal/osal_c/interfaces
    ${c_CMPT_AUDIO_DIR}/base/libs/src/include
    ${c_CMPT_UTILS_DIR}/ipc/rpc/include/xdr
    ${c_CMPT_UTILS_DIR}/ipc/rpc/src/aidl
    ${c_CMPT_UTILS_DIR}/ipc/rpc/tests/aidl
)
# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

###NOTE: For open-source component, always build from source
ameba_add_internal_library(rpc_test
    p_SOURCES
        ${private_sources}
    p_INCLUDES
        ${private_includes}
    p_DEFINITIONS
        ${private_definitions}
    p_COMPILE_OPTIONS
        ${private_compile_options}
)
##########################################################################################