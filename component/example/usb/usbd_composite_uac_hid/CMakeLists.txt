ameba_internal_library(example_usbd_composite__uac_hid)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE

    example_usbd_composite_uac_hid.c
    app_example.c
)

target_include_directories(
    ${CURRENT_LIB_NAME} PRIVATE

    ${BASEDIR}/component/usb/common
    ${BASEDIR}/component/usb/device/core
    ${BASEDIR}/component/usb/device/composite

	${BASEDIR}/component/audio/interfaces
	${BASEDIR}/component/audio/base/osal/osal_c/interfaces
)

