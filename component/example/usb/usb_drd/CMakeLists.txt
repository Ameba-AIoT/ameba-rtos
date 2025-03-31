ameba_internal_library(example_usb_drd)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE

    example_usb_drd.c
    app_example.c
)

target_include_directories(
    ${CURRENT_LIB_NAME} PRIVATE

    ${BASEDIR}/component/usb/common
    ${BASEDIR}/component/usb/device/core
    ${BASEDIR}/component/usb/device/cdc_acm
    ${BASEDIR}/component/usb/device/composite
    ${BASEDIR}/component/usb/device/hid
    ${BASEDIR}/component/usb/device/msc
    ${BASEDIR}/component/usb/device/uac
    ${BASEDIR}/component/usb/device/vendor
    ${BASEDIR}/component/usb/host/core
    ${BASEDIR}/component/usb/host/cdc_acm
    ${BASEDIR}/component/usb/host/cdc_ecm
    ${BASEDIR}/component/usb/host/msc
    ${BASEDIR}/component/usb/host/vendor
    ${BASEDIR}/component/usb/host/uvc
)

