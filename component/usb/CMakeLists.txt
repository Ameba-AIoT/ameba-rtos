if(NOT CONFIG_SUPPORT_USB_DEVICE_ONLY)
    include(lib.cmake)
endif()

ameba_add_subdirectory(device)
ameba_add_subdirectory_if_exist(host)

if(CONFIG_USB_DRD_EN)
    include(usb.cmake)
endif()
