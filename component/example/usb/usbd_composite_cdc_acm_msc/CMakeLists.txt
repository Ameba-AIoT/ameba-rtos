ameba_internal_library(example_usbd_composite_cdc_acm_msc)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE

    example_usbd_composite_cdc_acm_msc.c
    app_example.c
)

target_include_directories(
    ${CURRENT_LIB_NAME} PRIVATE

    ${BASEDIR}/component/usb/common
    ${BASEDIR}/component/usb/device/core
    ${BASEDIR}/component/usb/device/composite
)

