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

if(CONFIG_AMEBAL2)
    ameba_list_append(private_definitions CONFIG_USB_FS)
endif()

ameba_list_append(private_includes
    ../common
    core
)

if(CONFIG_USBH_CDC_ACM)
    ameba_list_append(private_includes cdc_acm)
    ameba_list_append(private_sources cdc_acm/usbh_cdc_acm.c)
endif()

if(CONFIG_USBH_CDC_ECM)
    ameba_list_append(private_includes cdc_ecm)
    ameba_list_append(private_sources cdc_ecm/usbh_cdc_ecm.c cdc_ecm/usbh_cdc_ecm_hal.c)
endif()


if(CONFIG_USBH_MSC)
    ameba_list_append(private_includes msc)
    ameba_list_append(private_sources
        msc/usbh_msc.c
        msc/usbh_msc_scsi.c
        msc/usbh_msc_disk.c
    )
    if (CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
        if("${c_MCU_PROJECT_NAME}" STREQUAL "ap")
            ameba_list_append(private_sources msc/usbh_msc_cmd.c)
        endif()
    endif()
endif()

if(CONFIG_USBH_UVC)
    ameba_list_append(private_includes uvc)
    ameba_list_append(private_sources
        uvc/usbh_uvc_class.c
        uvc/usbh_uvc_intf.c
        uvc/usbh_uvc_parse.c
        uvc/usbh_uvc_stream.c
    )
    if (CONFIG_AMEBAGREEN2 OR CONFIG_AMEBAL2)
        ameba_list_append(private_sources uvc/usbh_uvc_dec.c)
    endif()
endif()

if(CONFIG_USBH_VENDOR)
    ameba_list_append(private_includes vendor)
    ameba_list_append(private_sources vendor/usbh_vendor.c)
endif()

# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

###NOTE: For open-source component, always build from source
ameba_add_internal_library(usb_host
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