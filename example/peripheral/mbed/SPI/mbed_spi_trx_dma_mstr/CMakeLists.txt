##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin

# Component public part, user config end
#----------------------------------------#

#WARNING: Fixed section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_definitions})
ameba_global_library(${public_libraries}) #default: whole-archived

##########################################################################################
## * This part defines private part of the component
## * Private part is used to build target of current component

set(private_sources)
set(private_includes)
set(private_definitions)
set(private_compile_options)

#------------------------------#
# Component private part, user config begin

ameba_list_append(private_sources
    example_mbed_spi_trx_dma_mstr.c
    app_example.c
)

# Component private part, user config end
#------------------------------#

ameba_add_internal_library(example_mbed_spi_trx_dma_mstr
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
