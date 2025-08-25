ameba_internal_library(example_atcmd_usbh_host)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_atcmd_usbd_host.c
    app_example.c
)

target_include_directories(
    ${CURRENT_LIB_NAME} PRIVATE

    ${BASEDIR}/component/usb/common
    ${BASEDIR}/component/usb/device/core
    ${BASEDIR}/component/usb/device/cdc_acm
)
