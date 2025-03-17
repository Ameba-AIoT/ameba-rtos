set(HAL_DIR  ${TARGETDIR}/fwlib/ram_common)
set(DRV_DIR  ${BASEDIR}/component/usb)
set(COM_DIR  ${DRV_DIR}/common)
set(LIB_DIR  ${DRV_DIR}/device/core)

set(
    MODULE_IFLAGS
    ${COM_DIR}
    ${LIB_DIR}
)

set(MODULE_C_DEFINES CONFIG_USB_FS)

set(
    CSRC
    ${HAL_DIR}/ameba_usb.c
	${COM_DIR}/usb_hal.c
	${COM_DIR}/usb_os.c
	${LIB_DIR}/usbd.c
	${LIB_DIR}/usbd_core.c
	${LIB_DIR}/usbd_hal.c
	${LIB_DIR}/usbd_pcd.c
)

ameba_app_library(usbd)


target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_DEFINES})