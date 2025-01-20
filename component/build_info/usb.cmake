if (__INCLUDED_USB_BUILD_CMAKE__)
    return()
else()
    set(__INCLUDED_USB_BUILD_CMAKE__ TRUE)
endif()

add_library(src_${d_MCU_PROJECT_NAME}_usbd_class INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_usbd_lib INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_usb_class INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_usb_lib INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_usbh_class INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_usbh_lib INTERFACE)

#--------------------------#
if(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
    if("${d_MCU_PROJECT_NAME}" STREQUAL "ap" OR "${d_MCU_PROJECT_NAME}" STREQUAL "hp")
        ameba_target_include(src_${d_MCU_PROJECT_NAME}_usbd_class p_SCOPE interface
            ${c_CMPT_USB_DIR}/common
            ${c_CMPT_USB_DIR}/device/core
        )
        ameba_target_set_if(CONFIG_USBD_CDC_ACM src_${d_MCU_PROJECT_NAME}_usbd_class p_SCOPE interface
            p_INCLUDES
                ${c_CMPT_USB_DIR}/device/cdc_acm
            p_SOURCES
                ${c_CMPT_USB_DIR}/device/cdc_acm/usbd_cdc_acm.c
        )
        ameba_target_include_if(CONFIG_USBD_COMPOSITE src_${d_MCU_PROJECT_NAME}_usbd_class p_SCOPE interface
            ${c_CMPT_USB_DIR}/device/composite
        )
        ameba_target_set_if(CONFIG_USBD_HID src_${d_MCU_PROJECT_NAME}_usbd_class p_SCOPE interface
            p_INCLUDES
                ${c_CMPT_USB_DIR}/device/hid
            p_SOURCES
                ${c_CMPT_USB_DIR}/device/hid/usbd_hid.c
        )
        ameba_target_set_if(CONFIG_USBD_MSC src_${d_MCU_PROJECT_NAME}_usbd_class p_SCOPE interface
            p_INCLUDES
                ${c_CMPT_USB_DIR}/device/msc
            p_SOURCES
                ${c_CMPT_USB_DIR}/device/msc/usbd_msc.c
                ${c_CMPT_USB_DIR}/device/msc/usbd_scsi.c
        )
        if("${d_MCU_PROJECT_NAME}" STREQUAL "ap")
            ameba_target_sources_if(CONFIG_USBD_MSC src_${d_MCU_PROJECT_NAME}_usbd_class p_SCOPE interface
                ${c_CMPT_USB_DIR}/device/msc/usbd_msc_cmd.c
            )
        endif()
        ameba_target_set_if(CONFIG_USBD_UAC src_${d_MCU_PROJECT_NAME}_usbd_class p_SCOPE interface
            p_INCLUDES
                ${c_CMPT_USB_DIR}/device/uac
            p_SOURCES
                ${c_CMPT_USB_DIR}/device/uac/usbd_uac.c
        )
        ameba_target_set_if(CONFIG_USBD_VENDOR src_${d_MCU_PROJECT_NAME}_usbd_class p_SCOPE interface
            p_INCLUDES
                ${c_CMPT_USB_DIR}/device/vendor
            p_SOURCES
                ${c_CMPT_USB_DIR}/device/vendor/usbd_vendor.c
        )
        ameba_target_set_if(CONFIG_USBD_VERIFY src_${d_MCU_PROJECT_NAME}_usbd_class p_SCOPE interface
            p_INCLUDES
                ${c_CMPT_USB_DIR}/verification/device
                ${c_CMPT_USB_DIR}/device/verify
            p_SOURCES
                ${c_CMPT_USB_DIR}/device/vendor/usbd_vendor.c
        )
        ameba_target_sources_if(CONFIG_USBD_COMPOSITE_CDC_ACM_HID src_${d_MCU_PROJECT_NAME}_usbd_class p_SCOPE interface
            ${c_CMPT_USB_DIR}/device/composite/usbd_composite_cdc_acm_hid.c
            ${c_CMPT_USB_DIR}/device/composite/usbd_composite_cdc_acm.c
            ${c_CMPT_USB_DIR}/device/composite/usbd_composite_hid.c
        )
        ameba_target_sources_if(CONFIG_USBD_COMPOSITE_CDC_ACM_UAC src_${d_MCU_PROJECT_NAME}_usbd_class p_SCOPE interface
            ${c_CMPT_USB_DIR}/device/composite/usbd_composite_cdc_acm_uac.c
            ${c_CMPT_USB_DIR}/device/composite/usbd_composite_cdc_acm.c
            ${c_CMPT_USB_DIR}/device/composite/usbd_composite_uac.c
        )
    endif()
endif()
#--------------------------#
ameba_target_include(src_${d_MCU_PROJECT_NAME}_usbd_lib p_SCOPE interface
    ${c_CMPT_USB_DIR}/common
    ${c_CMPT_USB_DIR}/device/core
)
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_usbd_lib p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_usb.c
    ${c_CMPT_USB_DIR}/common/usb_hal.c
    ${c_CMPT_USB_DIR}/common/usb_os.c
    ${c_CMPT_USB_DIR}/device/core/usbd.c
    ${c_CMPT_USB_DIR}/device/core/usbd_core.c
    ${c_CMPT_USB_DIR}/device/core/usbd_hal.c
    ${c_CMPT_USB_DIR}/device/core/usbd_pcd.c
)
#--------------------------#
if(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
    if("${d_MCU_PROJECT_NAME}" STREQUAL "ap" OR "${d_MCU_PROJECT_NAME}" STREQUAL "hp")
        ameba_target_include(src_${d_MCU_PROJECT_NAME}_usb_class p_SCOPE interface
            ${c_CMPT_USB_DIR}/common
            ${c_CMPT_USB_DIR}/device/core
            ${c_CMPT_USB_DIR}/host/core
        )

        ameba_target_set_if(CONFIG_USBD_CDC_ACM src_${d_MCU_PROJECT_NAME}_usb_class p_SCOPE interface
            p_INCLUDES
                ${c_CMPT_USB_DIR}/device/cdc_acm
            p_SOURCES
                ${c_CMPT_USB_DIR}/device/cdc_acm/usbd_cdc_acm.c
        )
        ameba_target_set_if(CONFIG_USBD_COMPOSITE src_${d_MCU_PROJECT_NAME}_usb_class p_SCOPE interface
            p_INCLUDES
                ${c_CMPT_USB_DIR}/device/composite
            p_SOURCES
                ${c_CMPT_USB_DIR}/device/composite/usbd_composite_cdc_acm_hid.c
                ${c_CMPT_USB_DIR}/device/composite/usbd_composite_cdc_acm.c
                ${c_CMPT_USB_DIR}/device/composite/usbd_composite_hid.c
        )
        ameba_target_set_if(CONFIG_USBD_HID src_${d_MCU_PROJECT_NAME}_usb_class p_SCOPE interface
            p_INCLUDES
                ${c_CMPT_USB_DIR}/device/hid
            p_SOURCES
                ${c_CMPT_USB_DIR}/device/hid/usbd_hid.c
        )
        ameba_target_set_if(CONFIG_USBD_MSC src_${d_MCU_PROJECT_NAME}_usb_class p_SCOPE interface
            p_INCLUDES
                ${c_CMPT_USB_DIR}/device/msc
            p_SOURCES
                ${c_CMPT_USB_DIR}/device/msc/usbd_msc.c
                ${c_CMPT_USB_DIR}/device/msc/usbd_scsi.c
        )
        if("${d_MCU_PROJECT_NAME}" STREQUAL "ap")
            ameba_target_set_if(CONFIG_USBD_UAC src_${d_MCU_PROJECT_NAME}_usb_class p_SCOPE interface
                p_INCLUDES
                    ${c_CMPT_USB_DIR}/device/uac
                p_SOURCES
                    ${c_CMPT_USB_DIR}/device/uac/usbd_uac.c
            )
        endif()
        ameba_target_set_if(CONFIG_USBD_VENDOR src_${d_MCU_PROJECT_NAME}_usb_class p_SCOPE interface
            p_INCLUDES
                ${c_CMPT_USB_DIR}/device/vendor
            p_SOURCES
                ${c_CMPT_USB_DIR}/device/vendor/usbd_vendor.c
        )
        ameba_target_set_if(CONFIG_USBD_VERIFY src_${d_MCU_PROJECT_NAME}_usb_class p_SCOPE interface
            p_INCLUDES
                ${c_CMPT_USB_DIR}/verification/device
                ${c_CMPT_USB_DIR}/device/verify
            p_SOURCES
                ${c_CMPT_USB_DIR}/device/verify/usbd_verify.c
                ${c_CMPT_USB_DIR}/verification/device/usbd_cts.c
                ${c_CMPT_USB_DIR}/verification/device/usbd_cdc_acm_test.c
                ${c_CMPT_USB_DIR}/verification/device/usbd_cmd.c
                ${c_CMPT_USB_DIR}/verification/device/usbd_verify_test.c
        )
        ameba_target_set_if(CONFIG_USBH_CDC_ACM src_${d_MCU_PROJECT_NAME}_usb_class p_SCOPE interface
            p_INCLUDES
                ${c_CMPT_USB_DIR}/host/cdc_acm
            p_SOURCES
                ${c_CMPT_USB_DIR}/host/cdc_acm/usbh_cdc_acm.c
        )
        ameba_target_set_if(CONFIG_USBH_CDC_ECM src_${d_MCU_PROJECT_NAME}_usb_class p_SCOPE interface
            p_INCLUDES
                ${c_CMPT_USB_DIR}/host/cdc_ecm
            p_SOURCES
                ${c_CMPT_USB_DIR}/host/cdc_ecm/usbh_cdc_ecm.c
                ${c_CMPT_USB_DIR}/host/cdc_ecm/usbh_cdc_ecm_hal.c
        )
        ameba_target_set_if(CONFIG_USBH_MSC src_${d_MCU_PROJECT_NAME}_usb_class p_SCOPE interface
            p_INCLUDES
                ${c_CMPT_USB_DIR}/host/msc
            p_SOURCES
                ${c_CMPT_USB_DIR}/host/msc/usbh_msc.c
                ${c_CMPT_USB_DIR}/host/msc/usbh_msc_scsi.c
                ${c_CMPT_USB_DIR}/host/msc/usbh_msc_disk.c
        )
        ameba_target_set_if(CONFIG_USBH_UVC src_${d_MCU_PROJECT_NAME}_usb_class p_SCOPE interface
            p_INCLUDES
                ${c_CMPT_USB_DIR}/host/uvc
            p_SOURCES
                ${c_CMPT_USB_DIR}/host/uvc/usbh_uvc_class.c
                ${c_CMPT_USB_DIR}/host/uvc/usbh_uvc_intf.c
                ${c_CMPT_USB_DIR}/host/uvc/usbh_uvc_parse.c
                ${c_CMPT_USB_DIR}/host/uvc/usbh_uvc_stream.c
        )
        ameba_target_set_if(CONFIG_USBH_VENDOR src_${d_MCU_PROJECT_NAME}_usb_class p_SCOPE interface
            p_INCLUDES
                ${c_CMPT_USB_DIR}/host/vendor
            p_SOURCES
                ${c_CMPT_USB_DIR}/host/vendor/usbh_vendor.c
        )
        ameba_target_set_if(CONFIG_USBH_VERIFY src_${d_MCU_PROJECT_NAME}_usb_class p_SCOPE interface
            p_INCLUDES
                ${c_CMPT_USB_DIR}/host/verify
                ${c_CMPT_USB_DIR}/verification/host
            p_SOURCES
                ${c_CMPT_USB_DIR}/host/verify/usbh_verify.c
                ${c_CMPT_USB_DIR}/verification/host/usbh_cts.c
                ${c_CMPT_USB_DIR}/verification/host/usbh_cts_xfer_test.c
                ${c_CMPT_USB_DIR}/verification/host/usbh_emc.c
                ${c_CMPT_USB_DIR}/verification/host/usbh_cmd.c
                ${c_CMPT_USB_DIR}/verification/host/usbh_verify_test.c
        )
    endif()
endif()
#--------------------------#
ameba_target_include(src_${d_MCU_PROJECT_NAME}_usb_lib p_SCOPE interface
    ${c_CMPT_USB_DIR}/common
    ${c_CMPT_USB_DIR}/device/core
    ${c_CMPT_USB_DIR}/host/core
)
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_usb_lib p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_usb.c
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
#--------------------------#
if(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
    if("${d_MCU_PROJECT_NAME}" STREQUAL "ap" OR "${d_MCU_PROJECT_NAME}" STREQUAL "hp")
        ameba_target_include(src_${d_MCU_PROJECT_NAME}_usbh_class p_SCOPE interface
            ${c_CMPT_USB_DIR}/common
            ${c_CMPT_USB_DIR}/host/core
        )
        ameba_target_set_if(CONFIG_USBH_CDC_ACM src_${d_MCU_PROJECT_NAME}_usbh_class p_SCOPE interface
            p_INCLUDES
                ${c_CMPT_USB_DIR}/host/cdc_acm
            p_SOURCES
                ${c_CMPT_USB_DIR}/host/cdc_acm/usbh_cdc_acm.c
        )
        ameba_target_set_if(CONFIG_USBH_CDC_ECM src_${d_MCU_PROJECT_NAME}_usbh_class p_SCOPE interface
            p_INCLUDES
                ${c_CMPT_USB_DIR}/host/cdc_ecm
            p_SOURCES
                ${c_CMPT_USB_DIR}/host/cdc_ecm/usbh_cdc_ecm.c
                ${c_CMPT_USB_DIR}/host/cdc_ecm/usbh_cdc_ecm_hal.c
        )
        if("${d_MCU_PROJECT_NAME}" STREQUAL "ap")
            ameba_target_set_if(CONFIG_USBH_MSC src_${d_MCU_PROJECT_NAME}_usbh_class p_SCOPE interface
                p_INCLUDES
                    ${c_CMPT_USB_DIR}/host/msc
                p_SOURCES
                    ${c_CMPT_USB_DIR}/host/msc/usbh_msc.c
                    ${c_CMPT_USB_DIR}/host/msc/usbh_msc_scsi.c
                    ${c_CMPT_USB_DIR}/host/msc/usbh_msc_disk.c
                    ${c_CMPT_USB_DIR}/host/msc/usbh_msc_cmd.c
            )
        endif()
        ameba_target_set_if(CONFIG_USBH_UVC src_${d_MCU_PROJECT_NAME}_usbh_class p_SCOPE interface
            p_INCLUDES
                ${c_CMPT_USB_DIR}/host/uvc
            p_SOURCES
                ${c_CMPT_USB_DIR}/host/uvc/usbh_uvc_class.c
                ${c_CMPT_USB_DIR}/host/uvc/usbh_uvc_intf.c
                ${c_CMPT_USB_DIR}/host/uvc/usbh_uvc_parse.c
                ${c_CMPT_USB_DIR}/host/uvc/usbh_uvc_stream.c
        )
        ameba_target_set_if(CONFIG_USBH_VENDOR src_${d_MCU_PROJECT_NAME}_usbh_class p_SCOPE interface
            p_INCLUDES
                ${c_CMPT_USB_DIR}/host/vendor
            p_SOURCES
                ${c_CMPT_USB_DIR}/host/vendor/usbh_vendor.c
        )
        ameba_target_set_if(CONFIG_USBH_VERIFY src_${d_MCU_PROJECT_NAME}_usbh_class p_SCOPE interface
            p_INCLUDES
                ${c_CMPT_USB_DIR}/host/verify
                ${c_CMPT_USB_DIR}/verification/host
            p_SOURCES
                ${c_CMPT_USB_DIR}/host/verify/usbh_verify.c
                ${c_CMPT_USB_DIR}/verification/host/usbh_cts.c
                ${c_CMPT_USB_DIR}/verification/host/usbh_cts_xfer_test.c
                ${c_CMPT_USB_DIR}/verification/host/usbh_emc.c
                ${c_CMPT_USB_DIR}/verification/host/usbh_cmd.c
                ${c_CMPT_USB_DIR}/verification/host/usbh_verify_test.c
        )
    endif()
endif()
#--------------------------#
ameba_target_include(src_${d_MCU_PROJECT_NAME}_usbh_lib p_SCOPE interface
    ${c_CMPT_USB_DIR}/common
    ${c_CMPT_USB_DIR}/host/core
)
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_usbh_lib p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_usb.c
    ${c_CMPT_USB_DIR}/common/usb_hal.c
    ${c_CMPT_USB_DIR}/common/usb_os.c
    ${c_CMPT_USB_DIR}/host/core/usbh.c
    ${c_CMPT_USB_DIR}/host/core/usbh_core.c
    ${c_CMPT_USB_DIR}/host/core/usbh_hal.c
    ${c_CMPT_USB_DIR}/host/core/usbh_hcd.c
)
#--------------------------#
