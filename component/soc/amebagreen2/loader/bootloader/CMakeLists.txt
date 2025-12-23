##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin

ameba_list_append(public_includes
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Component public part, user config end
#----------------------------------------#

#WARNING: Fixed section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_definitions})
ameba_global_library(${public_libraries}) #default: whole-archived

##########################################################################################
## * This part defines private part of the component
## * Private part is used to build target of current component
## * NOTE: The build API guarantees the global build configures(mentioned above)
## *       applied to the target automatically. So if any configure was already added
## *       to public above, it's unnecessary to add again below.

#NOTE: User defined section, add your private build configures here
# You may use if-else condition to set these predefined variable
# They are only for ameba_add_internal_library/ameba_add_external_app_library/ameba_add_external_soc_library
set(private_sources)                 #private source files, NOTE: relative path is OK
set(private_includes)                #private include directories, NOTE: relative path is OK
set(private_definitions)             #private definitions
set(private_compile_options)         #private compile_options

#------------------------------#
# Component private part, user config begin

if("${c_MCU_PROJECT_NAME}" STREQUAL "km4ns")
    ameba_list_append(private_sources
        bootloader_km4ns.c
    )
elseif("${c_MCU_PROJECT_NAME}" STREQUAL "km4tz")
    ameba_list_append(private_sources
        bootloader_km4tz.c
        boot_ota_km4tz.c
        boot_security_km4tz.c
        bootloader_clk.c
        bootloader_hci.c
    )
endif()

if ((NOT CONFIG_WHC_INTF_IPC) AND CONFIG_WHC_DEV)
  set(loader_condition TRUE)
else()
  set(loader_condition FALSE)
endif()

if("${c_MCU_PROJECT_NAME}" STREQUAL "km4tz" AND loader_condition)
    ameba_list_append_if(loader_condition private_includes
        hci/core
    )

    ameba_list_append_if(CONFIG_WHC_INTF_USB private_includes
        ${c_CMPT_USB_DIR}/common
        hci/usb
    )

    ameba_list_append_if(CONFIG_WHC_INTF_SDIO private_includes
        hci/sdio
    )

    ameba_list_append_if(loader_condition private_sources
        hci/core/hci_core.c
    )

    ameba_list_append_if(CONFIG_WHC_INTF_USB private_sources
        hci/usb/usbd_core.c
        hci/usb/usbd_hal.c
        hci/usb/usbd_pcd.c
        hci/usb/usbd_otp.c
        hci/usb/usbd_fullmac_class.c
        hci/usb/usbd_intf.c
    )

    ameba_list_append_if(CONFIG_WHC_INTF_SDIO private_sources
        hci/sdio/sdio_intf.c
        hci/sdio/sdio_hal.c
    )
endif()

# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

###NOTE: For open-source component, always build from source
ameba_add_internal_library(bootloader_internal
    p_SOURCES
        ${private_sources}
    p_INCLUDES
        ${private_includes}
    p_DEFINITIONS
        ${private_definitions}
    p_COMPILE_OPTIONS
        ${private_compile_options}
)
##########################################################################################
