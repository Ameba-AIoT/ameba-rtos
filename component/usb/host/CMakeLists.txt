if (NOT CONFIG_SUPPORT_USB_DEVICE_ONLY)
    include(lib.cmake)
endif()

if(CONFIG_USB_HOST_EN)
    include(usbh.cmake)
endif()
