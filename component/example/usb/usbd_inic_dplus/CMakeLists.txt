ameba_internal_library(example_usbd_inic)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE

    example_usbd_inic_dplus.c
    app_example.c
)

target_include_directories(
    ${CURRENT_LIB_NAME} PRIVATE

    ${BASEDIR}/component/usb/common
    ${BASEDIR}/component/usb/device/core
    ${BASEDIR}/component/usb/device/inic_dplus

)

