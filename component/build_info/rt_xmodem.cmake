if (__INCLUDED_RT_XMODEM_BUILD_CMAKE__)
    return()
else()
    set(__INCLUDED_RT_XMODEM_BUILD_CMAKE__ TRUE)
endif()

add_library(src_${d_MCU_PROJECT_NAME}_rt_xmodem INTERFACE)

#--------------------------#
ameba_target_set(src_${d_MCU_PROJECT_NAME}_rt_xmodem p_SCOPE interface
    p_INCLUDES
        ${d_SOC_PLATFORM_DIR}/app/xmodem
        ${d_SOC_PLATFORM_DIR}/fwlib/include
        ${c_COMPONENT_DIR}/mbed/hal
        ${d_SOC_PLATFORM_DIR}/hal/include
        ${d_SOC_PLATFORM_DIR}/hal/src
        ${c_COMPONENT_DIR}/soc/common/flash_download/rt_xmodem
        ${d_SOC_PLATFORM_DIR}/swlib
        ${c_CMPT_USB_DIR}/common
        ${c_COMPONENT_DIR}/soc/common/flash_download/usb_download
        ${c_COMPONENT_DIR}/soc/common/imgtool_floader/src/../inc
    p_SOURCES
        ${c_COMPONENT_DIR}/soc/common/flash_download/rt_xmodem/floader_fw_update.c
        ${c_COMPONENT_DIR}/soc/common/flash_download/rt_xmodem/floader_xmodem.c
        ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_reset.c
        ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_otp.c
        ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_ipc_api.c
        ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_tim.c
        ${d_SOC_PLATFORM_DIR}/lib/ram_common/ameba_rom_patch.c
        ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_usb.c
        ${d_SOC_PLATFORM_DIR}/fwlib/ram_hp/ram_hp/ameba_codec.c
        ${d_SOC_PLATFORM_DIR}/swlib/log.c
        ${c_COMPONENT_DIR}/soc/common/imgtool_floader/src/ameba_floader_rom_parse.c
        ${d_SOC_PLATFORM_DIR}/hal/src/pinmap_common.c
        ${d_SOC_PLATFORM_DIR}/hal/src/pinmap.c
        ${d_SOC_PLATFORM_DIR}/hal/src/gpio_api.c
        ${d_SOC_PLATFORM_DIR}/hal/src/pwmout_api.c
        ${c_COMPONENT_DIR}/soc/common/flash_download/usb_download/usbd_cdc.c
        ${c_COMPONENT_DIR}/soc/common/flash_download/usb_download/usbd_core.c
        ${c_COMPONENT_DIR}/soc/common/flash_download/usb_download/usbd_dbg.c
        ${c_COMPONENT_DIR}/soc/common/flash_download/usb_download/usbd_hal.c
        ${c_COMPONENT_DIR}/soc/common/flash_download/usb_download/usbd_intf.c
        ${c_COMPONENT_DIR}/soc/common/flash_download/usb_download/usbd_pcd.c
        ${c_CMPT_USB_DIR}/common/usb_hal.c
        ${c_CMPT_USB_DIR}/common/usb_os.c
        ${c_COMPONENT_DIR}/soc/common/imgtool_floader/src/floader_nand_ftl.c
        ${c_COMPONENT_DIR}/soc/common/imgtool_floader/src/floader_nand_ftl_mfg.c
        ${c_COMPONENT_DIR}/soc/common/imgtool_floader/src/floader_nor_ftl.c
)

#--------------------------#