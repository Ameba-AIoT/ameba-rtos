ameba_internal_library(example_usbd_rndis)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE

    example_usbd_rndis.c
    app_example.c
)

target_include_directories(
    ${CURRENT_LIB_NAME} PRIVATE

    ${BASEDIR}/component/usb/common
    ${BASEDIR}/component/usb/device/core
    ${BASEDIR}/component/usb/device/rndis
)
