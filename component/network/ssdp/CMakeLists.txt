##########################################################################################
## * This part defines public part of the component

set(public_includes)
set(public_definitions)
set(public_libraries)

ameba_list_append(public_includes
    ${c_CMPT_NETWORK_DIR}/ssdp
)

ameba_global_include(${public_includes})
ameba_global_define(${public_definitions})
ameba_global_library(${public_libraries})

##########################################################################################
## * This part defines private part of the component

set(private_sources)
set(private_includes)
set(private_definitions)
set(private_compile_options)

ameba_list_append(private_sources
    ssdp.c
)

ameba_add_internal_library(ssdp
    p_SOURCES
        ${private_sources}
    p_INCLUDES
        ${private_includes}
    p_DEFINITIONS
        ${private_definitions}
    p_COMPILE_OPTIONS
        ${private_compile_options}
)
