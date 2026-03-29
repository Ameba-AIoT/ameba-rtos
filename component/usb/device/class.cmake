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

ameba_list_append_if(CONFIG_USBD_COMPOSITE_CDC_ACM_HID private_sources
    ${USBD_CLASS_DIR}/composite/usbd_composite_cdc_acm_hid.c
    ${USBD_CLASS_DIR}/composite/usbd_composite_cdc_acm.c
    ${USBD_CLASS_DIR}/composite/usbd_composite_hid.c
)

ameba_list_append_if(CONFIG_USBD_COMPOSITE_CDC_ACM_MSC private_sources
    ${USBD_CLASS_DIR}/composite/usbd_composite_cdc_acm_msc.c
    ${USBD_CLASS_DIR}/composite/usbd_composite_cdc_acm.c
    ${USBD_CLASS_DIR}/composite/usbd_composite_msc.c
    ${USBD_CLASS_DIR}/composite/usbd_composite_scsi.c
)

if(CONFIG_USBD_COMPOSITE_CDC_ACM_UAC1 OR CONFIG_USBD_COMPOSITE_HID_UAC1)
    set(COMPOSITE_UAC_SOURCE ${USBD_CLASS_DIR}/composite/usbd_composite_uac1.c)
else()
    set(COMPOSITE_UAC_SOURCE ${USBD_CLASS_DIR}/composite/usbd_composite_uac2.c)
endif()

ameba_list_append_if(CONFIG_USBD_COMPOSITE_CDC_ACM_UAC private_sources
    ${USBD_CLASS_DIR}/composite/usbd_composite_cdc_acm_uac.c
    ${USBD_CLASS_DIR}/composite/usbd_composite_cdc_acm.c
    ${COMPOSITE_UAC_SOURCE}
)

ameba_list_append_if(CONFIG_USBD_COMPOSITE_HID_UAC private_sources
    ${USBD_CLASS_DIR}/composite/usbd_composite_hid_uac.c
    ${USBD_CLASS_DIR}/composite/usbd_composite_hid_bi_dir.c
    ${COMPOSITE_UAC_SOURCE}
)
