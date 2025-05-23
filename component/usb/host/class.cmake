# Class-specific configuration

set(USBH_CLASS_DIR "${CMAKE_CURRENT_LIST_DIR}")

ameba_list_append(private_includes
    ${USBH_CLASS_DIR}/../common
    ${USBH_CLASS_DIR}/core
)

if(CONFIG_USBH_CDC_ACM)
    ameba_list_append(private_includes
        ${USBH_CLASS_DIR}/cdc_acm
    )
    ameba_list_append(private_sources
        ${USBH_CLASS_DIR}/cdc_acm/usbh_cdc_acm.c
    )
endif()

if(CONFIG_USBH_CDC_ECM)
    ameba_list_append(private_includes
        ${USBH_CLASS_DIR}/cdc_ecm
    )
    ameba_list_append(private_sources
        ${USBH_CLASS_DIR}/cdc_ecm/usbh_cdc_ecm.c
        ${USBH_CLASS_DIR}/cdc_ecm/usbh_cdc_ecm_hal.c
    )
endif()


if(CONFIG_USBH_MSC)
    ameba_list_append(private_includes
        ${USBH_CLASS_DIR}/msc
    )
    ameba_list_append(private_sources
        ${USBH_CLASS_DIR}/msc/usbh_msc.c
        ${USBH_CLASS_DIR}/msc/usbh_msc_scsi.c
        ${USBH_CLASS_DIR}/msc/usbh_msc_disk.c
    )
endif()

if(CONFIG_USBH_UVC)
    ameba_list_append(private_includes
        ${USBH_CLASS_DIR}/uvc
    )
    ameba_list_append(private_sources
        ${USBH_CLASS_DIR}/uvc/usbh_uvc_class.c
        ${USBH_CLASS_DIR}/uvc/usbh_uvc_intf.c
        ${USBH_CLASS_DIR}/uvc/usbh_uvc_parse.c
        ${USBH_CLASS_DIR}/uvc/usbh_uvc_stream.c
    )
    if (CONFIG_SUPPORT_USBH_UVC_HW_DEC)
        ameba_list_append(private_sources
            ${c_CMPT_SOC_DIR}/fwlib/ram_common/ameba_uvc.c
            ${USBH_CLASS_DIR}/uvc/usbh_uvc_dec.c
        )
    endif()
endif()

if(CONFIG_USBH_VENDOR)
    ameba_list_append(private_includes
        ${USBH_CLASS_DIR}/vendor
    )
    ameba_list_append(private_sources
        ${USBH_CLASS_DIR}/vendor/usbh_vendor.c
    )
endif()
