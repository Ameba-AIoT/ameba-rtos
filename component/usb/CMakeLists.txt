ameba_add_subdirectory(device)
ameba_add_subdirectory_if_exist(host)

if (NOT CONFIG_SUPPORT_USB_DEVICE_ONLY)
    include(drd_lib.cmake)
endif()

if(CONFIG_USB_DRD_EN)
    include(drd_class.cmake)
endif()
