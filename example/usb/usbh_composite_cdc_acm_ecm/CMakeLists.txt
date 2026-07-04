ameba_internal_library(example_usbh_composite_cdc_acm_ecm)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE

    example_usbh_composite_cdc_acm_ecm.c
    app_example.c
)

target_include_directories(
    ${CURRENT_LIB_NAME} PRIVATE

    ${BASEDIR}/component/usb/common
    ${BASEDIR}/component/usb/host/core
    ${BASEDIR}/component/usb/host/composite
)

