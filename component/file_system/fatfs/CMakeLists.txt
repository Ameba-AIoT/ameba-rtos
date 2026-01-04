##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#------------------------------------------------------------------#
# Component public part, user config begin(DO NOT remove this line)
# You may use if-else condition to set or update predefined variable above

ameba_list_append(public_includes
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/${v_FATFS_VER}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../vfs
)

# Component public part, user config end(DO NOT remove this line)
#------------------------------------------------------------------#

#WARNING: Fixed section, DO NOT change!
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
set(private_sources)                 #private source files, NOTE: relative path is OK
set(private_includes)                #private include directories, NOTE: relative path is OK
set(private_definitions)             #private definitions
set(private_compile_options)         #private compile_options

#------------------------------------------------------------------#
# Component private part, user config begin(DO NOT remove this line)

#NOTE: User defined section, add your private build configures here
# You may use if-else condition to set these predefined variable

ameba_list_append(private_compile_options
    -Wno-implicit-fallthrough
)

ameba_list_append(private_sources
    ${v_FATFS_VER}/src/ff.c
    ${v_FATFS_VER}/src/diskio.c
    ${v_FATFS_VER}/src/ffsystem.c
    ${v_FATFS_VER}/src/ffunicode.c
)

ameba_list_append(private_sources
    fatfs_flash.c
    fatfs_flash_api.c
    fatfs_sdcard.c
    fatfs_sdcard_api.c
    fatfs_usbh_api.c
)

# Component private part, user config end(DO NOT remove this line)
#------------------------------------------------------------------#

###NOTE: For open-source component, always build from source
ameba_add_internal_library(fatfs
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