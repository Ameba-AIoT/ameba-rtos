set(AMEBA_GLOBAL_DEFINED TRUE)
include(${CMAKE_CURRENT_LIST_DIR}/utility.cmake)

############################## Global Definitions Can be Override ################################
# Definition starts with v_ meas overridable variable
# Call ameba_reset_global_define() after reset variable below

ameba_set_if_unset(v_ASDK_VER 10.3.1)
ameba_set_if_unset(v_VSDK_VER 10.3.1)

ameba_set_if_unset(v_FATFS_VER r0.14b)
ameba_set_if_unset(v_FREERTOS_VER v10.4.3)
ameba_set_if_unset(c_FREERTOS_DIR_SUFFIX)

ameba_set_if_unset(v_LITTLEFS_VER r2.50)
ameba_set_if_unset(v_LWIP_VER v2.1.2)
ameba_set_if_unset(v_MBEDTLS_VER 3.6.2)
ameba_set_if_unset(v_HTTP2_VER 1.31.0)

ameba_set_if_unset(v_GIC_VER 2)

############################## Global Definitions MUST NOT be Override Manually ################################

ameba_set_basedir(c_BASEDIR)
ameba_set(c_CMAKE_FILES_DIR ${CMAKE_CURRENT_LIST_DIR})
ameba_set(c_CMAKE_BUILD_DIR)
ameba_set(c_COMPONENT_DIR ${c_BASEDIR}/component)
ameba_set(c_EMPTY_C_FILE ${CMAKE_CURRENT_LIST_DIR}/empty_file.c)
ameba_set(c_VALID_IMAGE_TYPES image1 image2 image3 gdb_floader imgtool_floader rom_ns rom_tz)

#An empty object library with empty_file.c but remove or rename some sections
ameba_set(c_EMPTY_C_OBJECT)         # File used like empty_file.c
ameba_set(c_EMPTY_C_OBJECT_FILE)    # File used like empty_file.c

#-------------------------#

# Define static dirs under component/
ameba_set(c_CMPT_AIVOICE_DIR      ${c_COMPONENT_DIR}/aivoice)
ameba_set(c_CMPT_APP_DIR          ${c_COMPONENT_DIR}/application)
ameba_set(c_CMPT_AT_CMD_DIR       ${c_COMPONENT_DIR}/at_cmd)
ameba_set(c_CMPT_AUDIO_DIR        ${c_COMPONENT_DIR}/audio)
ameba_set(c_CMPT_BLUETOOTH_DIR    ${c_COMPONENT_DIR}/bluetooth)
ameba_set(c_CMPT_DYN_APP_DIR      ${c_COMPONENT_DIR}/dynamic_app)
ameba_set(c_CMPT_ETHERNET_DIR     ${c_COMPONENT_DIR}/ethernet)
ameba_set(c_CMPT_EXAMPLE_DIR      ${c_COMPONENT_DIR}/example)
ameba_set(c_CMPT_FILE_SYSTEM_DIR  ${c_COMPONENT_DIR}/file_system)
ameba_set(c_CMPT_LWIP_DIR         ${c_COMPONENT_DIR}/lwip)
ameba_set(c_CMPT_NETWORK_DIR      ${c_COMPONENT_DIR}/network)
ameba_set(c_CMPT_OS_DIR           ${c_COMPONENT_DIR}/os)
ameba_set(c_CMPT_RTK_COEX_DIR     ${c_COMPONENT_DIR}/rtk_coex)
ameba_set(c_CMPT_SDIO_DIR         ${c_COMPONENT_DIR}/sdio)
ameba_set(c_CMPT_SSL_DIR          ${c_COMPONENT_DIR}/ssl)
ameba_set(c_CMPT_UI_DIR           ${c_COMPONENT_DIR}/ui)
ameba_set(c_CMPT_USB_DIR          ${c_COMPONENT_DIR}/usb)
ameba_set(c_CMPT_UTILS_DIR        ${c_COMPONENT_DIR}/utils)
ameba_set(c_CMPT_TFLITE_DIR       ${c_COMPONENT_DIR}/tflite_micro)
ameba_set(c_CMPT_WIFI_DIR         ${c_COMPONENT_DIR}/wifi)
ameba_set(c_CMPT_WPAN_DIR         ${c_COMPONENT_DIR}/wpan)

ameba_set(c_CMPT_CRASHDUMP_DIR    ${c_COMPONENT_DIR}/soc/common/crashdump)
ameba_set(c_CMPT_LZMA_DIR         ${c_COMPONENT_DIR}/soc/common/lzma)
ameba_set(c_CMPT_ED25519_DIR      ${c_COMPONENT_DIR}/soc/common/rom_ed25519)
#-------------------------#

# Define dynamic dirs under component/
# NOTE: Feel free to use them. They will be updated to valid values automatically when project is created.
ameba_set(c_FREERTOS_DIR)
ameba_set(c_MBEDTLS_DIR)
ameba_set(c_LWIP_DIR)

#-------------------------#

# Define dynamic dirs under component/soc
# NOTE: Feel free to use them. They will be updated to valid values automatically when project is created.
ameba_set(c_CMPT_BOOTLOADER_DIR)
ameba_set(c_CMPT_CMSIS_DIR)
ameba_set(c_CMPT_CMSIS_DSP_DIR)
ameba_set(c_CMPT_FWLIB_DIR)
ameba_set(c_CMPT_HAL_DIR)
ameba_set(c_CMPT_IMG3_DIR)
ameba_set(c_CMPT_MISC_DIR)
ameba_set(c_CMPT_SWLIB_DIR)
ameba_set(c_CMPT_USRCFG_DIR)
ameba_set(c_CMPT_VECTOR_DIR)
ameba_set(c_CMPT_VERIFICATION_DIR)
ameba_set(c_CMPT_XMODEM_DIR)

#-------------------------#

# Define variables of soc project
# NOTE: Feel free to use them. They will be updated to valid values automatically when project is created.
ameba_set(c_SOC_PROJECT_DIR)    # SOC(IC) project root dir like amebasmart_gcc_project
ameba_set(c_SOC_TYPE)           # SOC(IC) type, like amebasmart, amebadplus, amebagreen2
ameba_set(c_SOC_TYPE_UPPER)     # SOC(IC) type in upper case, like AMEBASMART, AMEBADPLUS, AMEBAGREEN2
ameba_set(c_SOC_TYPE_CAMEL)     # SOC(IC) type in camel case, like AmebaSmart, AmebaDplus, AmebaGreen2
ameba_set(c_CMPT_SOC_DIR)       # PATH to the component/soc/${c_SOC_TYPE} directory.

#-------------------------#

# Define variables of mcu project
# NOTE: Feel free to use them. They will be updated to valid values automatically when project is created.
ameba_set(c_MCU_TYPE)                   # MCU type, like km0, km4, ca32, kr4
ameba_set(c_MCU_TYPE_UPPER)
ameba_set(c_MCU_PROJECT_DIR)
ameba_set(c_MCU_PROJECT_NAME)
ameba_set(c_MCU_PROJECT_NAME_UPPER)
ameba_set(c_MCU_KCONFIG_FILE)           # Kconfig file for current mcu project

ameba_set(c_SDK_NAME)
ameba_set(c_SDK_NAME_UPPER)
ameba_set(c_SDK_VERSION)

ameba_set(c_MCU_SDK_DIR)    # PATH to the ${c_MCU_PROJECT_DIR}/${c_SDK_NAME} directory.
ameba_set(c_MCU_SRC_DIR)    # PATH to the ${c_MCU_PROJECT_DIR}/src directory.
ameba_set(c_MCU_INC_DIR)    # PATH to the ${c_MCU_PROJECT_DIR}/inc directory.

# sub dirs in ${c_MCU_PROJECT_DIR}/*sdk/
ameba_set(c_SDK_BUILD_DIR)
ameba_set(c_SDK_FLASHLOADER_DIR)
ameba_set(c_SDK_GNU_UTILITY_DIR)
ameba_set(c_SDK_GNU_SCRIPT_DIR)
ameba_set(c_SDK_IMGTOOL_FLOADER_DIR)
ameba_set(c_SDK_IMAGE_DIR)
ameba_set(c_SDK_IMAGE_MP_DIR)
ameba_set(c_SDK_IMAGE_UTILITY_DIR)
ameba_set(c_SDK_LD_DIR)
ameba_set(c_SDK_LD_NS_DIR)
ameba_set(c_SDK_LIB_DIR)
ameba_set(c_SDK_LIB_APPLICATION_DIR)
ameba_set(c_SDK_LIB_APP_DIR)
ameba_set(c_SDK_LIB_SOC_DIR)

ameba_set(c_SDK_IMAGE_FOLDER_NAME)
ameba_set(c_SDK_IMAGE_TARGET_DIR)

# scripts in ${c_SDK_IMAGE_UTILITY_DIR}
ameba_set(c_SDK_ROM_SYMBOL_GEN_SCRIPT)
ameba_set(c_SDK_ROM_SYMBOL_S_GEN_SCRIPT)
ameba_set(c_SDK_ROM_TOTAL_SIZE_SCRIPT)
ameba_set(c_SDK_ROM_CODE_ANALYZE_SCRIPT)
ameba_set(c_SDK_EXTRACT_LD_SCRIPT)
############################################################

macro(ameba_reset_global_define)
    ameba_set(c_FREERTOS_DIR ${c_COMPONENT_DIR}/os/freertos/freertos_${v_FREERTOS_VER}${c_FREERTOS_DIR_SUFFIX}/Source)
    ameba_set(c_LWIP_DIR ${c_COMPONENT_DIR}/lwip/lwip_${v_LWIP_VER})
    ameba_set(c_MBEDTLS_DIR ${c_COMPONENT_DIR}/ssl/mbedtls-${v_MBEDTLS_VER})

    #Dirs below depend on ${c_CMPT_SOC_DIR}, which is set after sco project is created
    if(CONFIG_AMEBAL2 OR CONFIG_AMEBAGREEN2)
        ameba_set(c_CMPT_BOOTLOADER_DIR ${c_CMPT_SOC_DIR}/loader/bootloader)
    else()
        ameba_set(c_CMPT_BOOTLOADER_DIR ${c_CMPT_SOC_DIR}/bootloader)
    endif()
    ameba_set(c_CMPT_GDB_FLOADER_DIR ${c_CMPT_SOC_DIR}/loader/gdb_floader)
    ameba_set(c_CMPT_CMSIS_DIR ${c_CMPT_SOC_DIR}/cmsis)
    ameba_set(c_CMPT_CMSIS_DSP_DIR ${c_CMPT_SOC_DIR}/cmsis-dsp)
    ameba_set(c_CMPT_FWLIB_DIR ${c_CMPT_SOC_DIR}/fwlib)
    ameba_set(c_CMPT_HAL_DIR ${c_CMPT_SOC_DIR}/hal)
    ameba_set(c_CMPT_IMG3_DIR ${c_CMPT_SOC_DIR}/img3)
    ameba_set(c_CMPT_MISC_DIR ${c_CMPT_SOC_DIR}/misc)
    ameba_set(c_CMPT_MVE_DIR ${c_CMPT_SOC_DIR}/mve_lib)
    ameba_set(c_CMPT_SWLIB_DIR ${c_CMPT_SOC_DIR}/swlib)
    ameba_set(c_CMPT_USRCFG_DIR ${c_COMPONENT_DIR}/soc/usrcfg/${c_SOC_TYPE})
    ameba_set(c_CMPT_VECTOR_DIR ${c_CMPT_SOC_DIR}/vector)
    ameba_set(c_CMPT_VERIFICATION_DIR ${c_CMPT_SOC_DIR}/verification)
    ameba_set(c_CMPT_CHIPINFO_DIR ${c_CMPT_SOC_DIR}/lib/chipinfo)
    ameba_set(c_CMPT_MONITOR_DIR ${c_CMPT_SOC_DIR}/app/monitor)
    ameba_set(c_CMPT_DOWNLOAD_XMODEM_DIR ${c_CMPT_SOC_DIR}/rom/download/xmodem)
    ameba_set(c_CMPT_DOWNLOAD_USB_DIR ${c_CMPT_SOC_DIR}/rom/download/usb)
    ameba_set(c_CMPT_PMC_DIR ${c_CMPT_SOC_DIR}/lib/pmc)
    ameba_set(c_CMPT_ROM_DIR ${c_CMPT_SOC_DIR}/rom)
endmacro()

############################################################

ameba_reset_global_define()
