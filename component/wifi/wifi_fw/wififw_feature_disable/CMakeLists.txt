##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin

#----------------------------------------#
# Component public part, user config end
#----------------------------------------#

#WARNING: Fixed section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_defines})
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

if(CONFIG_AMEBAD)
    return()
endif()

set(WIFIFWDIR ${c_CMPT_WIFI_DIR}/wifi_fw)

if(NOT CONFIG_AMEBASMART)
    ameba_list_append_ifnot(CONFIG_WIFI_CSI_ENABLE private_sources
        wififw_csi_disable_com.c)
endif()

ameba_list_append(private_includes
    ${WIFIFWDIR}/common
)

if(CONFIG_AMEBAPRO3)
    ameba_list_append(private_includes ${WIFIFWDIR}/amebagreen2/include)
elseif(CONFIG_RLE1509)
    ameba_list_append(private_includes ${WIFIFWDIR}/rle1509/include)
elseif(CONFIG_RTL8720F)
    ameba_list_append(private_includes ${WIFIFWDIR}/rtl8720f/include)
elseif(CONFIG_AMEBALITE)
    ameba_list_append(private_includes ${WIFIFWDIR}/amebalite/include)
else()
    ameba_list_append(private_includes ${WIFIFWDIR}/${c_SOC_TYPE}/include)
endif()

# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

if(private_sources)
    ameba_add_internal_library(wififw_feature_disable
        p_NO_SEAL
        p_SOURCES
            ${private_sources}
        p_INCLUDES
            ${private_includes}
        p_DEFINITIONS
            ${private_definitions}
        p_COMPILE_OPTIONS
            ${private_compile_options}
    )
endif()
##########################################################################################
