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
if (CONFIG_MEDIA_PLAYER)
    ameba_list_append(public_libraries
        ${c_SDK_LIB_APPLICATION_DIR}/lib_gsm.a
    )
endif()

list(REMOVE_DUPLICATES public_libraries)
# Component public part, user config end
#----------------------------------------#

#WARNING: Fixed section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_definitions})
ameba_global_library(${public_libraries} p_NO_WHOLE_ARCHIVE) #NOTE: no_whole-archived

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
ameba_list_append(private_includes
    inc
)
ameba_list_append(private_sources
    src/add.c
    src/code.c
    src/decode.c
    src/gsm_create.c
    src/gsm_decode.c
    src/gsm_destroy.c
    src/gsm_encode.c
    src/gsm_option.c
    src/long_term.c
    src/lpc.c
    src/preprocess.c
    src/rpe.c
    src/short_term.c
    src/table.c
)
ameba_list_append(private_definitions
    __RTOS__
    __ SASR
    WAV49
    WMOPS=1
)
ameba_list_append(private_compile_options
    -Wno-error
)

# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

###NOTE: For closed-source component, only build before release and libs are packaged into lib/application
ameba_add_external_app_library(gsm
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