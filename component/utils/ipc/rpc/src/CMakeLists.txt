##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin

# You may use if-else condition to set or update predefined variable above
ameba_list_append(public_includes
    ${c_CMPT_UTILS_DIR}/ipc/ipc/${c_SOC_TYPE}/include
    ${c_CMPT_UTILS_DIR}/ipc/rpc/hal/interfaces
    ${c_CMPT_UTILS_DIR}/ipc/rpc/include
    ${c_CMPT_UTILS_DIR}/ipc/rpc/xdr/include
)

ameba_list_append_if(public_libraries
    ${c_SDK_LIB_APPLICATION_DIR}/lib_rpc.a
)

## Differentiated configuration based on SoC types

# Component public part, user config end
#----------------------------------------#

#WARNING: Fixed section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_defines})
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
    rpc_client.c
    rpc_server.c
    rpc_common.c
    rpc_utils.c
    xdr/xdr_array.c
    xdr/xdr.c
    xdr/xdr_float.c
    xdr/xdr_mem.c
    xdr/xdr_process.c
    xdr/xdr_ref.c
    xdr/xdr_sizeof.c
)
ameba_list_append(private_definitions
    __RTOS__
)
ameba_list_append(private_compile_options
    -Wimplicit-function-declaration
)
# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

###NOTE: For closed-source component, only build before release and libs are packaged into lib/application
ameba_add_external_app_library(rpc
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