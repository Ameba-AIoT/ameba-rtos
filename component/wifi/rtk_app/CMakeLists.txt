##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin

# You may use if-else condition to set or update predefined variable above

set(TARGET_LIB wifi_rtk_app)
set(TARGET_LIB_OPEN wifi_rtk_app_open)

ameba_list_append(public_includes
    ${CMAKE_CURRENT_SOURCE_DIR}/wifi_cast
    ${CMAKE_CURRENT_SOURCE_DIR}/wifi_fast_connect
    ${CMAKE_CURRENT_SOURCE_DIR}/wifi_auto_reconnect
    ${CMAKE_CURRENT_SOURCE_DIR}/wifi_tunnel_app
    ${CMAKE_CURRENT_SOURCE_DIR}/wifi_radar
)

set(need_external_lib OFF)

if(CONFIG_WHC_NONE OR CONFIG_WHC_HOST)
    if(NOT CONFIG_MP_SHRINK)
        set(need_external_lib ON)
    endif()
endif()

if(CONFIG_PLATFORM_ZEPHYR)
    set(need_external_lib ON)
endif()

ameba_list_append_if(need_external_lib public_libraries ${c_SDK_LIB_APPLICATION_DIR}/lib_${TARGET_LIB}.a)

# NAN host control app is shipped closed-source: link the prebuilt lib_nan.a
# (built from source before release by ameba_add_external_app_library below).
ameba_list_append_if(CONFIG_WIFI_NAN_HOST_APP public_libraries ${c_SDK_LIB_APPLICATION_DIR}/lib_nan.a)

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

set(private_sources_open)                 #private source files, NOTE: relative path is OK
set(private_includes_open)                #private include directories, NOTE: relative path is OK
set(private_definitions_open)             #private definitions
set(private_compile_options_open)         #private compile_options

#------------------------------#
# Component private part, user config begin
if(NOT CONFIG_MP_SHRINK)
    if(CONFIG_WHC_NONE OR CONFIG_WHC_HOST)
        ameba_list_append(private_sources wifi_acs/wifi_acs.c)
        ameba_list_append(private_sources wifi_radar/wifi_radar.c)
        if (CONFIG_TFLITE_MICRO_EN)
            ameba_list_append(private_sources wifi_radar/nn_interface.cc)
            ameba_list_append(private_sources wifi_radar/models/radar_float.c)
            ameba_list_append(private_definitions TF_LITE_STATIC_MEMORY)
            ameba_list_append(private_compile_options -Wno-unused-parameter -Wno-undef)
        endif()
    endif()
    if(NOT CONFIG_PLATFORM_ZEPHYR)
        ameba_list_append_if(CONFIG_WHC_HOST private_sources wifi_cast/wifi_intf_drv_to_app_cast.c)
    endif()
endif()

if(CONFIG_PLATFORM_ZEPHYR)
    ameba_list_append(private_sources wifi_init/wifi_init.c)
    ameba_list_append(private_sources wifi_zephyr/wifi_zephyr_to_driver.c)
    if(CONFIG_WHC_HOST OR CONFIG_WHC_NONE)
        ameba_list_append(private_sources wifi_auto_reconnect/wifi_auto_reconnect.c)
        ameba_list_append(private_sources wifi_fast_connect/wifi_fast_connect.c)
    endif()
else()
    ameba_list_append(private_sources_open wifi_init/wifi_init.c)
    if(CONFIG_WHC_HOST OR CONFIG_WHC_NONE)
        ameba_list_append(private_sources_open wifi_auto_reconnect/wifi_auto_reconnect.c)
        ameba_list_append(private_sources_open wifi_fast_connect/wifi_fast_connect.c)
    endif()
endif()

# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

###NOTE: For closed-source component, only build before release and libs are packaged into lib/application
if(need_external_lib)
    ameba_add_external_app_library(${TARGET_LIB}
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

###NOTE: For open-source component, always build from source
ameba_add_internal_library(${TARGET_LIB_OPEN}
    p_DROP_IF_NO_SOURCES
    p_SOURCES
        ${private_sources_open}
    p_INCLUDES
        ${private_includes_open}
    p_DEFINITIONS
        ${private_definitions_open}
    p_COMPILE_OPTIONS
        ${private_compile_options_open}
)

# NAN host control app (km4_tz). Built as a CLOSED-SOURCE prebuilt lib (lib_nan.a)
# so this ported code is not shipped as source: ameba_add_external_app_library
# compiles it before release into lib/application and the public section links
# the prebuilt .a (on release the source is dropped). The warning policy for this
# ported-from-Linux code stays isolated here; the -Wno-error relaxations cover
# legacy style only (unused vars, K&R prototypes, char/uint8 signedness,
# debug-print formats); revisit for a cleanup pass.
if(CONFIG_WIFI_NAN_HOST_APP)
    ameba_add_external_app_library(nan
        p_SOURCES
            wifi_nan_utility/rtw_nan_cmd.c
            wifi_nan_utility/nan_event.c
            wifi_nan_utility/sha256.c
            wifi_nan_utility/nan_app.c
        p_INCLUDES
            ${CMAKE_CURRENT_SOURCE_DIR}/wifi_nan_utility
        p_DEFINITIONS
            CONFIG_WIFI_NAN_HOST_APP
        p_COMPILE_OPTIONS
            -Wno-error=unused-variable
            -Wno-error=unused-parameter
            -Wno-error=strict-prototypes
            -Wno-error=pointer-sign
            -Wno-error=format
            -Wno-error=address
    )
endif()
##########################################################################################

if(NOT CONFIG_PLATFORM_ZEPHYR AND NOT CONFIG_MP_SHRINK)
if(CONFIG_WHC_HOST OR (CONFIG_WHC_DEV AND NOT CONFIG_WHC_INTF_IPC))
     ameba_add_subdirectory(wifi_tunnel_app)
endif()
endif()

ameba_add_subdirectory(wifi_feature_disable)
