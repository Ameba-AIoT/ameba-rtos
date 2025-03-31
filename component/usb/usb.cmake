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

if(CONFIG_AMEBAL2)
    ameba_list_append(private_definitions CONFIG_USB_FS)
endif()

ameba_list_append(private_includes
    common
    device/core
    host/core
)

if(CONFIG_USBD_CDC_ACM)
    ameba_list_append(private_includes device/cdc_acm)
    ameba_list_append(private_sources device/cdc_acm/usbd_cdc_acm.c)
endif()

if(CONFIG_USBD_COMPOSITE)
    ameba_list_append(private_includes device/composite)
    ameba_list_append(private_sources
        device/composite/usbd_composite_cdc_acm_hid.c
        device/composite/usbd_composite_cdc_acm.c
        device/composite/usbd_composite_hid.c
    )
endif()

if(CONFIG_USBD_HID)
    ameba_list_append(private_includes device/hid)
    ameba_list_append(private_sources device/hid/usbd_hid.c)
endif()

if(CONFIG_USBD_MSC)
    ameba_list_append(private_includes device/msc)
    ameba_list_append(private_sources
        device/msc/usbd_msc.c
        device/msc/usbd_scsi.c
    )
endif()

if(CONFIG_USBD_UAC)
    if(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
        if("${c_MCU_PROJECT_NAME}" STREQUAL "ap")
            ameba_list_append(private_includes device/uac)
            ameba_list_append(private_sources device/uac/usbd_uac.c)
        endif()
    endif()
endif()

if(CONFIG_USBD_VENDOR)
    ameba_list_append(private_includes device/vendor)
    ameba_list_append(private_sources device/vendor/usbd_vendor.c)
endif()

if(CONFIG_USBH_CDC_ACM)
    ameba_list_append(private_includes host/cdc_acm)
    ameba_list_append(private_sources host/cdc_acm/usbh_cdc_acm.c)
endif()

if(CONFIG_USBH_CDC_ECM)
    ameba_list_append(private_includes host/cdc_ecm)
    ameba_list_append(private_sources host/cdc_ecm/usbh_cdc_ecm.c host/cdc_ecm/usbh_cdc_ecm_hal.c)
endif()


if(CONFIG_USBH_MSC)
    ameba_list_append(private_includes host/msc)

    ameba_list_append(private_sources
        host/msc/usbh_msc.c
        host/msc/usbh_msc_scsi.c
        host/msc/usbh_msc_disk.c
    )

endif()

if(CONFIG_USBH_UVC)
    ameba_list_append(private_includes host/uvc)
    ameba_list_append(private_sources
        host/uvc/usbh_uvc_class.c
        host/uvc/usbh_uvc_intf.c
        host/uvc/usbh_uvc_parse.c
        host/uvc/usbh_uvc_stream.c
    )
endif()

if(CONFIG_USBH_VENDOR)
    ameba_list_append(private_includes host/vendor)
    ameba_list_append(private_sources host/vendor/usbh_vendor.c)
endif()

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
)
##########################################################################################