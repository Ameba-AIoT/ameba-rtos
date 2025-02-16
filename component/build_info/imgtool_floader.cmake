if (__INCLUDED_IMGTOOL_FLOADER_BUILD_CMAKE__)
    return()
else()
    set(__INCLUDED_IMGTOOL_FLOADER_BUILD_CMAKE__ TRUE)
endif()

add_library(src_${d_MCU_PROJECT_NAME}_imgtool_floader INTERFACE)

#--------------------------#
if(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
    if("${d_MCU_PROJECT_NAME}" STREQUAL "hp")
        ameba_target_set(src_${d_MCU_PROJECT_NAME}_imgtool_floader p_SCOPE interface
            p_COMPILE_DEFINES
                CONFIG_NAND_DOWNLOAD_EN
                CONFIG_FLOADER_USBD_EN
            p_INCLUDES
                ${d_SOC_PLATFORM_DIR}/app/xmodem
                ${d_SOC_PLATFORM_DIR}/fwlib/include
                ${c_COMPONENT_DIR}/mbed/hal
                ${d_SOC_PLATFORM_DIR}/hal/include
                ${d_SOC_PLATFORM_DIR}/hal/src
                ${d_SOC_PLATFORM_DIR}/swlib
                ${c_CMPT_USB_DIR}/common
                ${c_COMPONENT_DIR}/soc/common/flash_download/imgtool_floader
                ${c_COMPONENT_DIR}/soc/common/flash_wrapper/inc
            p_SOURCES
                ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_reset.c
                ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_otp.c
                ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_ipc_api.c
                ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_tim.c
                # ${d_SOC_PLATFORM_DIR}/lib/ram_common/ameba_rom_patch.c # this file also included by lib_chipinfo.a
                ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_usb.c
                ${d_SOC_PLATFORM_DIR}/fwlib/ram_hp/ameba_codec.c
                ${d_SOC_PLATFORM_DIR}/swlib/log.c
                ${c_CMPT_USB_DIR}/common/usb_hal.c
                ${c_CMPT_USB_DIR}/common/usb_os.c
                ${c_COMPONENT_DIR}/soc/common/flash_download/imgtool_floader/usbd_cdc.c
                ${c_COMPONENT_DIR}/soc/common/flash_download/imgtool_floader/usbd_core.c
                ${c_COMPONENT_DIR}/soc/common/flash_download/imgtool_floader/usbd_hal.c
                ${c_COMPONENT_DIR}/soc/common/flash_download/imgtool_floader/usbd_intf.c
                ${c_COMPONENT_DIR}/soc/common/flash_download/imgtool_floader/usbd_pcd.c
                ${c_COMPONENT_DIR}/soc/common/flash_download/imgtool_floader/loguart_intf.c
                ${c_COMPONENT_DIR}/soc/common/flash_download/imgtool_floader/floader_debug.c
                ${c_COMPONENT_DIR}/soc/common/flash_download/imgtool_floader/floader_fb.c
                ${c_COMPONENT_DIR}/soc/common/flash_download/imgtool_floader/floader_intf.c
                ${c_COMPONENT_DIR}/soc/common/flash_download/imgtool_floader/floader.c
                ${c_COMPONENT_DIR}/soc/common/flash_wrapper/src/ameba_nand_ftl.c
                ${c_COMPONENT_DIR}/soc/common/flash_wrapper/src/ameba_nand_mfg_ftl.c
                ${c_COMPONENT_DIR}/soc/common/flash_wrapper/src/ameba_nor_ftl.c
        )
    endif()
endif()
#--------------------------#
