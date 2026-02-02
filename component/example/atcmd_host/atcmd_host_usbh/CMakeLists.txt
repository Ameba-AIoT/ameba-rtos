ameba_internal_library(example_atcmd_host_usbh)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_atcmd_host_usbh.c
    app_example.c
)

target_include_directories(
    ${CURRENT_LIB_NAME} PRIVATE

    ${BASEDIR}/component/usb/common
    ${BASEDIR}/component/usb/host/core
    ${BASEDIR}/component/usb/host/cdc_acm
)
