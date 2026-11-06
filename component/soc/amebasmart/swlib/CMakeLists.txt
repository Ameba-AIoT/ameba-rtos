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
    ${CMAKE_CURRENT_SOURCE_DIR}
)
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
    log.c
    sscanf_minimal.c

    libnosys/gettod.c
)

ameba_list_append(private_sources
    locks.c
)

# ASDK 12.3.1 libnosys stubs carry .gnu.warning sections, use warning-free stubs on LP/HP (AP has its own in fwlib/ap_core/cstub.c)
if(CONFIG_TOOLCHAIN_ASDK_12_3_1 AND NOT CONFIG_ARM_CORE_CA32)
    set(nosys_stubs
        libnosys/close.c
        libnosys/fstat.c
        libnosys/getpid.c
        libnosys/isatty.c
        libnosys/kill.c
        libnosys/lseek.c
    )
    # _read/_write are already provided by monitor/ram/low_level_io.c in hp image2
    if(NOT (c_MCU_PROJECT_NAME STREQUAL "hp" AND c_CURRENT_IMAGE_TYPE STREQUAL "image2"))
        list(APPEND nosys_stubs libnosys/read.c libnosys/write.c)
    endif()
    ameba_list_append(private_sources ${nosys_stubs})
    set_source_files_properties(${nosys_stubs} PROPERTIES COMPILE_OPTIONS -Wno-unused-parameter)
endif()
# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

###NOTE: For open-source component, always build from source
ameba_add_internal_library(swlib
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
