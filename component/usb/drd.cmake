##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin

# You may use if-else condition to set or update predefined variable above

#FIXME: miss role for public_libraries

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

ameba_list_append(private_includes
    ${c_CMPT_USB_DIR}/common
    ${c_CMPT_USB_DIR}/device/core
    ${c_CMPT_USB_DIR}/host/core
)

ameba_list_append(private_sources
    ${c_CMPT_SOC_DIR}/fwlib/ram_common/ameba_usb.c
    ${c_CMPT_USB_DIR}/common/usb_hal.c
    ${c_CMPT_USB_DIR}/common/usb_os.c
    ${c_CMPT_USB_DIR}/device/core/usbd.c
    ${c_CMPT_USB_DIR}/device/core/usbd_core.c
    ${c_CMPT_USB_DIR}/device/core/usbd_hal.c
    ${c_CMPT_USB_DIR}/device/core/usbd_pcd.c
    ${c_CMPT_USB_DIR}/host/core/usbh.c
    ${c_CMPT_USB_DIR}/host/core/usbh_core.c
    ${c_CMPT_USB_DIR}/host/core/usbh_hal.c
    ${c_CMPT_USB_DIR}/host/core/usbh_hcd.c
)

ameba_list_append_ifnot(CONFIG_SUPPORT_USB_NO_PHY private_sources
    ${c_CMPT_USB_DIR}/common/usb_phy.c
)

include(${c_CMPT_USB_DIR}/device/class.cmake)

include(${c_CMPT_USB_DIR}/host/class.cmake)

# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

###NOTE: For open-source component, always build from source
ameba_add_internal_library(usb_drd
    p_SOURCES
        ${private_sources}
    p_INCLUDES
        ${private_includes}
    p_DEFINITIONS
        ${private_definitions}
    p_COMPILE_OPTIONS
        ${private_compile_options}
    p_DROP_IF_NO_SOURCES
)
##########################################################################################
