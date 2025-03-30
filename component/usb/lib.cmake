##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin

# You may use if-else condition to set or update predefined variable above

ameba_list_append_if(CONFIG_USB_DRD_EN public_libraries
    ${c_SDK_LIB_APPLICATION_DIR}/lib_usb.a
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

if(CONFIG_AMEBAL2)
    ameba_list_append(private_definitions CONFIG_USB_FS)
endif()

ameba_list_append(private_includes
    common
    device/core
    host/core
)

ameba_list_append(private_sources
    ${c_CMPT_SOC_DIR}/fwlib/ram_common/ameba_usb.c
    common/usb_hal.c
    common/usb_os.c
    device/core/usbd.c
    device/core/usbd_core.c
    device/core/usbd_hal.c
    device/core/usbd_pcd.c
    host/core/usbh.c
    host/core/usbh_core.c
    host/core/usbh_hal.c
    host/core/usbh_hcd.c
)

# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

###NOTE: For closed-source component, only build before release and libs are packaged into lib/application
ameba_add_external_app_library(usb #target name, will be part of output library name: lib_usbd.a
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