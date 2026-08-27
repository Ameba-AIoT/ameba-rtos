# Class-specific configuration

set(USBD_CLASS_DIR "${CMAKE_CURRENT_LIST_DIR}")

ameba_list_append(private_includes
    ${USBD_CLASS_DIR}/../common
    ${USBD_CLASS_DIR}/core
)

if(CONFIG_USBD_CDC_ACM)
    ameba_list_append(private_includes
        ${USBD_CLASS_DIR}/cdc_acm
    )
    ameba_list_append(private_sources
        ${USBD_CLASS_DIR}/cdc_acm/usbd_cdc_acm.c
    )
endif()

if(CONFIG_USBD_CDC_ECM)
    ameba_list_append(private_includes
        ${USBD_CLASS_DIR}/cdc_ecm
    )
    ameba_list_append(private_sources
        ${USBD_CLASS_DIR}/cdc_ecm/usbd_cdc_ecm.c
    )
endif()

if(CONFIG_USBD_CDC_NCM)
    ameba_list_append(private_includes
        ${USBD_CLASS_DIR}/cdc_ncm
    )
    ameba_list_append(private_sources
        ${USBD_CLASS_DIR}/cdc_ncm/usbd_cdc_ncm.c
    )
endif()

ameba_list_append_if(CONFIG_USBD_COMPOSITE private_includes
    ${USBD_CLASS_DIR}/composite
)

if(CONFIG_USBD_HID)
    ameba_list_append(private_includes
        ${USBD_CLASS_DIR}/hid
    )
    ameba_list_append(private_sources
        ${USBD_CLASS_DIR}/hid/usbd_hid.c
    )
endif()

if(CONFIG_USBD_MSC)
    ameba_list_append(private_includes
        ${USBD_CLASS_DIR}/msc
    )
    ameba_list_append(private_sources
        ${USBD_CLASS_DIR}/msc/usbd_msc.c
        ${USBD_CLASS_DIR}/msc/usbd_scsi.c
    )
endif()

if(CONFIG_USBD_INIC)
    if(CONFIG_AMEBADPLUS)
        ameba_list_append(private_includes
            ${USBD_CLASS_DIR}/inic_dplus
        )
        ameba_list_append(private_sources
            ${USBD_CLASS_DIR}/inic_dplus/usbd_inic.c
        )
    else()
        ameba_list_append(private_includes
            ${USBD_CLASS_DIR}/inic
        )
        ameba_list_append(private_sources
            ${USBD_CLASS_DIR}/inic/usbd_inic.c
            ${USBD_CLASS_DIR}/inic/usbd_inic_otp.c
        )
    endif()
endif()

if(CONFIG_USBD_UAC)
    ameba_list_append(private_includes
        ${USBD_CLASS_DIR}/uac
    )
    if(CONFIG_USBD_UAC1)
        ameba_list_append(private_sources
            ${USBD_CLASS_DIR}/uac/usbd_uac1.c
        )
    else()
        ameba_list_append(private_sources
            ${USBD_CLASS_DIR}/uac/usbd_uac2.c
        )
    endif()
endif()

if(CONFIG_USBD_VENDOR)
    ameba_list_append(private_includes
        ${USBD_CLASS_DIR}/vendor
    )
    ameba_list_append(private_sources
        ${USBD_CLASS_DIR}/vendor/usbd_vendor.c
    )
endif()

if(CONFIG_USBD_DFU)
    ameba_list_append(private_includes
        ${USBD_CLASS_DIR}/dfu
    )
    ameba_list_append(private_sources
        ${USBD_CLASS_DIR}/dfu/usbd_dfu.c
    )
endif()

# New V6 composite framework: always built when CONFIG_USBD_COMPOSITE is enabled
# Sub-function class drivers (CDC ACM, HID, MSC, UAC, etc.) are each enabled
# by their own CONFIG_USBD_xxx and linked independently.
ameba_list_append_if(CONFIG_USBD_COMPOSITE private_sources
    ${USBD_CLASS_DIR}/composite/usbd_composite.c
)

if(CONFIG_USBD_UVC)
ameba_list_append(private_includes
        ${USBD_CLASS_DIR}/uvc
    )
ameba_list_append(private_sources
	${USBD_CLASS_DIR}/uvc/usbd_uvc.c
	${USBD_CLASS_DIR}/uvc/usbd_uvc_descriptor.c
	${USBD_CLASS_DIR}/uvc/usbd_uvc_event.c
)
endif()