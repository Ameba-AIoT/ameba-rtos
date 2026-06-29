# IC-specific configuration

set(USB_HAL_DIR "${CMAKE_CURRENT_LIST_DIR}")

ameba_list_append(private_includes
    ${USB_HAL_DIR}/
)

if(CONFIG_AMEBAD)
    ameba_list_append(private_sources
        ${USB_HAL_DIR}/amebad_usb.c
    )
endif()

if(CONFIG_AMEBADPLUS)
    ameba_list_append(private_sources
        ${USB_HAL_DIR}/amebadplus_usb.c
    )
endif()

if(CONFIG_AMEBAGREEN2)
    ameba_list_append(private_sources
        ${USB_HAL_DIR}/amebagreen2_usb.c
    )
endif()

if(CONFIG_AMEBAL2)
    ameba_list_append(private_sources
        ${USB_HAL_DIR}/amebaL2_usb.c
    )
endif()

if(CONFIG_AMEBAPRO3)
    ameba_list_append(private_sources
        ${USB_HAL_DIR}/amebapro3_usb.c
    )
endif()

if(CONFIG_AMEBASMART)
    ameba_list_append(private_sources
        ${USB_HAL_DIR}/amebasmart_usb.c
    )
endif()
