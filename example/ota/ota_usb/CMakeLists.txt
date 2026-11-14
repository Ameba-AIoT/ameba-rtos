ameba_internal_library(example_ota_usb)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_ota_usb.c
    app_example.c
)

# OTA_USB is built only on AmebaPro3 with the DFU class enabled. Define the same macro here so
# the example can report a clear error instead of failing at link time on other targets.
if(CONFIG_USBD_DFU_MENU AND CONFIG_USBD_DFU AND CONFIG_AMEBAPRO3)
    target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE OTA_USB_TRANSPORT)
endif()
