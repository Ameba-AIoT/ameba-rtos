if (CONFIG_AMEBAGREEN2 OR CONFIG_AMEBAL2)
    if("${c_MCU_PROJECT_NAME}" STREQUAL "km4tz")
        include(lib.cmake)
    endif()
elseif (NOT CONFIG_AMEBADPLUS)
    include(lib.cmake)
else()

endif()

if(CONFIG_USB_HOST_EN)
    include(usbh.cmake)
endif()
