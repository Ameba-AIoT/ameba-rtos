##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#------------------------------------------------------------------#
# Component public part, user config begin(DO NOT remove this line)
# You may use if-else condition to set or update predefined variable above

ameba_list_append(public_includes
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/${v_LITTLEFS_VER}
)

# Component public part, user config end(DO NOT remove this line)
#------------------------------------------------------------------#

#WARNING: Fixed section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_definitions})
ameba_global_library(${public_libraries}) #default: whole-archived

##########################################################################################
## * This part defines private part of the component
## * Private part is used to build target of current component
## * NOTE: The build API guarantees the global build configures(mentioned above)
## *       applied to the target automatically. So if any configure was already added
## *       to public, it's unnecessary to add again below.


# They are only for ameba_add_internal_library/ameba_add_external_app_library/ameba_add_external_soc_library
set(private_sources)                 #private source files, NOTE: relative path is OK
set(private_includes)                #private include directories, NOTE: relative path is OK
set(private_definitions)             #private definitions
set(private_compile_options)         #private compile_options

#------------------------------------------------------------------#
# Component private part, user config begin(DO NOT remove this line)

#NOTE: User defined section, add your private build configures here
# You may use if-else condition to set these predefined variable

ameba_list_append(private_sources
    littlefs_adapter.c
    ${v_LITTLEFS_VER}/lfs.c
    ${v_LITTLEFS_VER}/lfs_util.c
)
if(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
    ameba_list_append(private_sources
        lfs_nand_ftl.c
        lfs_nand_ftl_mfg.c
    )
    if("${c_MCU_PROJECT_NAME}" STREQUAL "ap")
        ameba_list_append(private_sources
            lfs_spinand.c
        )
    endif()
endif()

ameba_list_append(private_includes
    ${c_CMPT_FILE_SYSTEM_DIR}/ftl_common
)

# Component private part, user config end(DO NOT remove this line)
#------------------------------------------------------------------#

ameba_add_internal_library(littlefs
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