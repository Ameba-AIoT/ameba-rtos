ameba_internal_library(example_usb_drd)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE

    example_usb_drd.c
    usb_app_mgr.c
    usb_app_table.c
    app_example.c
)

# Every source of this library, its own and the apps', sees the aggregate build
target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE USB_APP_AGGREGATE)

target_include_directories(
    ${CURRENT_LIB_NAME} PRIVATE

    ${BASEDIR}/component/usb/common
    ${BASEDIR}/component/usb/device/core
    ${BASEDIR}/component/usb/host/core
)

# Every switchable app whose class is enabled in prj.conf is built in from its own
# example directory; its app_example.c is left out, this example owns the entry.
# USB_APP_AGGREGATE tells the app sources they share one image: an app hands its AT
# commands to the app manager instead of registering them itself.
# Triples: <app> <class Kconfig> <class include dir under component/usb>
# A composite app needs every class it combines, gated through a derived variable
if(CONFIG_USBD_COMPOSITE AND CONFIG_USBD_CDC_ACM AND CONFIG_USBD_HID)
    set(USB_DRD_COMP_ACM_HID ON)
endif()
if(CONFIG_USBD_COMPOSITE AND CONFIG_USBD_CDC_ACM AND CONFIG_USBD_MSC)
    set(USB_DRD_COMP_ACM_MSC ON)
endif()
if(CONFIG_USBD_COMPOSITE AND CONFIG_USBD_CDC_ACM AND CONFIG_USBD_UAC)
    set(USB_DRD_COMP_ACM_UAC ON)
endif()
if(CONFIG_USBD_COMPOSITE AND CONFIG_USBD_HID_VENDOR AND CONFIG_USBD_UAC)
    set(USB_DRD_COMP_HID_UAC ON)
endif()
if(CONFIG_USBH_HID AND CONFIG_USBH_UAC)
    set(USB_DRD_HOST_HID_UAC ON)
endif()
if(CONFIG_USBH_UVC AND CONFIG_USBH_UAC)
    set(USB_DRD_HOST_UVC_UAC ON)
endif()
# The 4G dongle option turns the ACM host class into a VID/PID matched dongle driver, which
# usbh_cdc_acm can not use; with it on, the ECM Ethernet host is the 4G composite instead of
# usbh_cdc_ecm. Bridge or Ethernet is a choice of the ECM host class.
if(CONFIG_USBH_CDC_ACM AND CONFIG_USBH_CDC_ACM_4G_DONGLE)
    set(USB_DRD_HOST_4G ON)
endif()
if(CONFIG_USBH_CDC_ACM AND NOT USB_DRD_HOST_4G)
    set(USB_DRD_HOST_ACM ON)
endif()
if(CONFIG_USBH_CDC_ECM AND CONFIG_USBH_ECM_ETHERNET AND NOT USB_DRD_HOST_4G)
    set(USB_DRD_HOST_ECM ON)
endif()
if(CONFIG_USBH_CDC_ECM AND CONFIG_USBH_ECM_ETHERNET AND USB_DRD_HOST_4G)
    set(USB_DRD_HOST_ACM_ECM ON)
endif()
if(CONFIG_USBH_CDC_ECM AND CONFIG_USBH_ECM_ETHERNET_BRIDGE)
    set(USB_DRD_HOST_BRIDGE ON)
endif()
set(USB_DRD_APPS
    usbd_cdc_acm CONFIG_USBD_CDC_ACM device/cdc_acm
    usbd_vendor  CONFIG_USBD_VENDOR  device/vendor
    usbd_dfu     CONFIG_USBD_DFU     device/dfu
    usbd_msc     CONFIG_USBD_MSC     device/msc
    usbh_cdc_acm USB_DRD_HOST_ACM    host/cdc_acm
    usbh_vendor  CONFIG_USBH_VENDOR  host/vendor
    usbh_dfu     CONFIG_USBH_DFU     host/dfu
    usbh_msc     CONFIG_USBH_MSC     host/msc
    usbh_hid     CONFIG_USBH_HID     host/hid
    usbd_cdc_ecm CONFIG_USBD_CDC_ECM device/cdc_ecm
    usbd_cdc_ncm CONFIG_USBD_CDC_NCM device/cdc_ncm
    usbh_cdc_ecm USB_DRD_HOST_ECM    host/cdc_ecm
    usbh_cdc_ncm CONFIG_USBH_CDC_NCM host/cdc_ncm
    usbd_hid     CONFIG_USBD_HID     device/hid
    usbd_uvc     CONFIG_USBD_UVC     device/uvc
    usbd_composite_cdc_acm_hid USB_DRD_COMP_ACM_HID device/composite
    usbd_composite_cdc_acm_msc USB_DRD_COMP_ACM_MSC device/composite
    usbd_uac     CONFIG_USBD_UAC     device/uac
    usbh_uac     CONFIG_USBH_UAC     host/uac
    usbh_uvc     CONFIG_USBH_UVC     host/uvc
    usbd_composite_cdc_acm_uac USB_DRD_COMP_ACM_UAC device/composite
    usbd_composite_hid_uac     USB_DRD_COMP_HID_UAC device/composite
    usbh_composite_hid_uac     USB_DRD_HOST_HID_UAC host/hid
    usbh_composite_uvc_uac     USB_DRD_HOST_UVC_UAC host/uvc
    usbh_composite_cdc_acm_ecm USB_DRD_HOST_ACM_ECM host/cdc_ecm
    usbh_wifi_bridge           USB_DRD_HOST_BRIDGE  host/cdc_ecm
)

list(LENGTH USB_DRD_APPS usb_drd_app_len)
math(EXPR usb_drd_app_last "${usb_drd_app_len} - 1")
foreach(i RANGE 0 ${usb_drd_app_last} 3)
    math(EXPR j "${i} + 1")
    math(EXPR k "${i} + 2")
    list(GET USB_DRD_APPS ${i} usb_drd_app)
    list(GET USB_DRD_APPS ${j} usb_drd_cfg)
    list(GET USB_DRD_APPS ${k} usb_drd_inc)
    if(${usb_drd_cfg})
        target_sources(${CURRENT_LIB_NAME} PRIVATE ${BASEDIR}/example/usb/${usb_drd_app}/example_${usb_drd_app}.c)
        target_include_directories(${CURRENT_LIB_NAME} PRIVATE
            ${BASEDIR}/example/usb/${usb_drd_app}
            ${BASEDIR}/component/usb/${usb_drd_inc}
        )
    endif()
endforeach()

# The 4G composite also drives the dongle AT port through the ACM host class
if(USB_DRD_HOST_ACM_ECM)
    target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${BASEDIR}/component/usb/host/cdc_acm)
endif()

# The audio apps play or record through the audio framework
if(CONFIG_USBD_UAC OR CONFIG_USBH_UAC)
    target_include_directories(${CURRENT_LIB_NAME} PRIVATE
        ${BASEDIR}/component/audio/interfaces
        ${BASEDIR}/component/audio/base/osal/osal_c/interfaces
    )
endif()
