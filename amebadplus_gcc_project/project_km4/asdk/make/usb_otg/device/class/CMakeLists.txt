set(DRV_DIR ${BASEDIR}/component/usb)
set(COM_DIR ${DRV_DIR}/common)
set(DEV_DIR ${DRV_DIR}/device)

set(
    MODULE_IFLAGS
    ${COM_DIR}
    ${DEV_DIR}/core
)

if(CONFIG_USBD_CDC_ACM)
    set(CDC_DIR ${DEV_DIR}/cdc_acm)
    list(
        APPEND MODULE_IFLAGS
        ${CDC_DIR}
    )
endif()

if(CONFIG_USBD_COMPOSITE)
    set(COMP_DIR ${DEV_DIR}/composite)
    list(
        APPEND MODULE_IFLAGS 
        ${COMP_DIR}
    )
endif()

if(CONFIG_USBD_HID)
    set(HID_DIR ${DEV_DIR}/hid)
    list(
        APPEND MODULE_IFLAGS 
        ${HID_DIR}
    )
endif()


if(CONFIG_USBD_VENDOR)
    set(VENDOR_DIR  ${DEV_DIR}/vendor)
    list(
        APPEND MODULE_IFLAGS 
        ${VENDOR_DIR}
    )
endif()

if(CONFIG_USBD_UAC)
    set(UAC_DIR  ${DEV_DIR}/uac)
    list(
        APPEND MODULE_IFLAGS 
        ${UAC_DIR}
    )
endif()


if(CONFIG_USBD_INIC)
    set(INIC_DIR ${DEV_DIR}/inic_dplus)
    list(
        APPEND MODULE_IFLAGS
        ${INIC_DIR}
    )
endif()

set(MODULE_C_DEFINES CONFIG_USB_FS)

set(CSRC)

if(CONFIG_USBD_CDC_ACM)
    list(
        APPEND CSRC 
        ${CDC_DIR}/usbd_cdc_acm.c
    )
endif()

if(CONFIG_USBD_COMPOSITE_CDC_ACM_HID)
    list(
        APPEND CSRC
        ${COMP_DIR}/usbd_composite_cdc_acm_hid.c 
        ${COMP_DIR}/usbd_composite_cdc_acm.c 
        ${COMP_DIR}/usbd_composite_hid.c
    )
endif()

if(CONFIG_USBD_HID)
    list(
        APPEND CSRC
        ${HID_DIR}/usbd_hid.c
    )
endif()

if(CONFIG_USBD_VENDOR)
    list(
        APPEND CSRC
        ${VENDOR_DIR}/usbd_vendor.c
    )
endif()

if(CONFIG_USBD_UAC)
    list(
        APPEND CSRC
        ${UAC_DIR}/usbd_uac.c
    )
endif()

if(CONFIG_USBD_INIC)
    list(
        APPEND CSRC
        ${INIC_DIR}/usbd_inic.c
    )
endif()


ameba_internal_library(usb_otg_device_class)
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_DEFINES})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
