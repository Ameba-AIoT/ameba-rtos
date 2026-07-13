##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#------------------------------------------------------------------#
# Component public part, user config begin(DO NOT remove this line)
# You may use if-else condition to set or update predefined variable above
ameba_list_append(public_includes ${CMAKE_CURRENT_SOURCE_DIR})

# Component public part, user config end(DO NOT remove this line)
#------------------------------------------------------------------#

#WARNING: Template section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_definitions})
ameba_global_library(${public_libraries})

##########################################################################################
## * This part defines private part of the component
## * Private part is used to build target of current component
## * NOTE: The build API guarantees the global build configures(mentioned above)
## *       applied to the target automatically. So if any configure was already added
## *       to public, it's unnecessary to add again below.

# They are only for ameba_add_internal_library/ameba_add_external_app_library/ameba_add_external_soc_library
set(private_sources)                 #private source files
set(private_includes)                #private include directories
set(private_definitions)             #private definitions
set(private_compile_options)         #private compile_options

#------------------------------------------------------------------#
# Component private part, user config begin(DO NOT remove this line)

#NOTE: User defined section, add your private build configures here
# You may use if-else condition to set these predefined variable
ameba_list_append(private_sources gcc_wrap.c)

ameba_list_append_if(CONFIG_TLSF_HEAP private_sources
    port_tlsf.c
    tlsf.c
)

if(CONFIG_TLSF_HEAP AND CONFIG_HEAP_TRACE)
    ameba_list_append(private_includes
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../os/freertos/heap_trace
    )
    # Same SDK_TOOLCHAIN / IMAGE_DIR defines as the freertos component, so the
    # malloc-failed backtrace in port_tlsf.c prints the exact addr2line command
    # heap_5.c does.
    file(RELATIVE_PATH IMAGE_DIR ${FINAL_IMAGE_DIR} ${c_SDK_IMAGE_TARGET_DIR})
    ameba_list_append(private_definitions
        SDK_TOOLCHAIN="${SDK_TOOLCHAIN}"
        IMAGE_DIR="${IMAGE_DIR}"
    )
endif()
# Component private part, user config end(DO NOT remove this line)
#------------------------------------------------------------------#

#WARNING: Template section, DO NOT change!
ameba_add_internal_library(libc_wrap
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
