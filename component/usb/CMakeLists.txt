if (CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
    include(lib.cmake)
endif()

ameba_add_subdirectory(device)
ameba_add_subdirectory_if_exist(host)

if(CONFIG_USB_DRD_EN)
    include(usb.cmake)
endif()
