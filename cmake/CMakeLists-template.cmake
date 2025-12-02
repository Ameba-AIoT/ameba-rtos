##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin

# You may use if-else condition to set or update predefined variable above
ameba_list_append(public_includes ${c_COMPONENT_DIR}/xxx) #simple append
ameba_list_append_if(CONFIG_XXX public_includes ${c_COMPONENT_DIR}/xxx) #conditional append
ameba_list_append_ifnot(CONFIG_XXX public_includes ${c_COMPONENT_DIR}/xxx) #conditional append
ameba_list_append(public_includes
    #Use relative path: from current CMakeLists.txt's dir
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/${v_LITTLEFS_VER}
)

## Differentiated configuration based on SoC types
if(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
     ameba_list_append_if(CONFIG_SPEEX_LIB public_libraries
         ${c_SDK_LIB_APPLICATION_DIR}/lib_speexdsp.a
     )
endif()

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
# They are only for ameba_add_internal_library
set(private_sources)                 #private source files, NOTE: relative path is OK
set(private_includes)                #private include directories, NOTE: relative path is OK
set(private_definitions)             #private definitions
set(private_compile_options)         #private compile_options

#------------------------------#
# Component private part, user config begin

ameba_list_append(private_sources lfs_nand_ftl_mfg.c)
ameba_list_append(private_includes
    ${CMAKE_CURRENT_SOURCE_DIR}
    common/amrnb/common/include
)

# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's build system

###NOTE: For open-source ameba component, always build from source
ameba_add_internal_library(at_cmd
    p_SOURCES
        ${private_sources}
    p_INCLUDES
        ${private_includes}
    p_DEFINITIONS
        ${private_definitions}
    p_COMPILE_OPTIONS
        ${private_compile_options}
)

###NOTE: For a third-party component with a standalone CMakeLists.txt, link the target to current image
ameba_add_subdirectory(path/to/your/cmakelists) # Add the real CMakeLists.txt dir of the wrapped component
ameba_port_standalone_internal_library(foo)     # Add the real target of the wrapped component to link

##########################################################################################
