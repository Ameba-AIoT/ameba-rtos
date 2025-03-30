##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#------------------------------------------------------------------#
# Component public part, user config begin(DO NOT remove this line)
# You may use if-else condition to set or update predefined variable above

ameba_list_append(public_includes include)
ameba_list_append(public_includes ../common)

# Component public part, user config end(DO NOT remove this line)
#------------------------------------------------------------------#

#WARNING: Fixed section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_definitions})
ameba_global_library(${public_libraries}) #default: whole-achived

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
if(${c_CURRENT_IMAGE_TYPE} STREQUAL "image1")
    ameba_list_append(private_sources
        ameba_boot_trustzonecfg.c
        ameba_bootcfg.c
        ameba_flashcfg.c
    )
elseif(${c_CURRENT_IMAGE_TYPE} STREQUAL "image2")
    ameba_list_append(private_sources
        ameba_sleepcfg.c
    )
    if(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
        if("${c_MCU_PROJECT_NAME}" STREQUAL "hp")
            ameba_list_append(private_sources
                ameba_pinmapcfg.c
                ameba_flashcfg.c
            )
        endif()
        if("${c_MCU_PROJECT_NAME}" STREQUAL "ap")
            ameba_list_append(private_sources
                ameba_flashcfg.c
            )       
        endif()
    endif()
endif()
# Component private part, user config end(DO NOT remove this line)
#------------------------------------------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone
###NOTE: For open-source component, always build from source
ameba_add_internal_library(usrcfg
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