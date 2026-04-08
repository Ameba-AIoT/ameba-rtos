ameba_add_subdirectory(device)
ameba_add_subdirectory_if_exist(host)

if(CONFIG_USB_DRD_EN)
    include(drd_lib.cmake)
    include(drd_class.cmake)
endif()
