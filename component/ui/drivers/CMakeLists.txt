##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin

# You may use if-else condition to set or update predefined variable above

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

# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

set(driver_list)

if(CONFIG_AMEBALITE)
    ameba_list_append(driver_list st7789v)
elseif(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
    ameba_list_append(driver_list cst328)
    ameba_list_append(driver_list st7701s)
elseif(CONFIG_AMEBADPLUS)
    ameba_list_append(driver_list ili9341)
elseif(CONFIG_AMEBAGREEN2)
    ameba_list_append(driver_list st7272a)
endif()

if(driver_list)
    ameba_add_subdirectories(${driver_list})

    ameba_add_merge_app_library(ui_drivers
        #WARNING: The name of the directory MUST be the same as
        #         the name of the target generated within it.
        ${driver_list}
    )
endif()
##########################################################################################



