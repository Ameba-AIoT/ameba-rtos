set(CMAKE_VERBOSE_MAKEFILE ON)

# ++++++++ Override variables depend on your project ++++++++ #
# - Full variables(and there default value) are defined in
#   make/global_define.cmake(starts with v_)

# set(v_ASDK_VER 10.3.1)
# set(v_VSDK_VER 10.3.1)
# set(v_FATFS_VER r0.14b)
set(v_FREERTOS_VER v10.2.1)
set(c_FREERTOS_DIR_SUFFIX _smp)
# set(v_LITTLEFS_VER r2.50)
# set(v_LWIP_VER v2.1.2)
# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #

# +++++++++++++++ Enable or disable dependency ++++++++++++++ #
# - Dependencies defined in make/dependency_common.cmake
# - Use ameba_set or ameba_set_if

ameba_set(d_DEP_AT_CMD ON)
ameba_set(d_DEP_AUDIO ON)
ameba_set(d_DEP_WIFI_AUDIO ON)
ameba_set_if(CONFIG_CLINTWOOD d_DEP_CLINTWOOD ON)
# ameba_set(d_DEP_EXAMPLE ON)
ameba_set_if(CONFIG_EMWIN_EN d_DEP_EMWIN ON)
ameba_set(d_DEP_FILE_SYSTEM ON)
ameba_set_if(CONFIG_TFLITE_MICRO_EN d_DEP_TFLITE_MICRO ON)
ameba_set_if(CONFIG_AIVOICE_EN d_DEP_AIVOICE ON)

ameba_set(d_DEP_OS ON)
ameba_set(d_DEP_SOC_COMMON ON)
ameba_set(d_DEP_SSL ON)
# ameba_set(d_DEP_STDLIB ON)
# ameba_set(d_DEP_USB_CDC_ECM ON)
ameba_set_if(CONFIG_AS_INIC_AP d_DEP_UTILITIES ON)
# ameba_set(d_DEP_UTILITIES_EXAMPLE ON)

# # - Dependencies defined in make/dependency_platform.cmake
ameba_set(d_DEP_APP_MONITOR ON)
# ameba_set(d_DEP_APP_XMODEM ON)
ameba_set(d_DEP_CMSIS ON)
# ameba_set(d_DEP_CMSIS_DSP ON)
ameba_set(d_DEP_FWLIB ON)
ameba_set(d_DEP_HAL ON)
# ameba_set(d_DEP_SWLIB ON)
# ameba_set(d_DEP_TEST ON)
ameba_set(d_DEP_USRCFG ON)
ameba_set(d_DEP_UTILS ON)

ameba_set(d_DEP_WIFI ON)
ameba_set(d_DEP_NETWORK ON)
if(CONFIG_AS_INIC_AP)
    ameba_set_if(d_DEP_UTILITIES ON)
elseif(CONFIG_AS_INIC_NP)
    ameba_set(d_DEP_RTK_COEX ON)
endif()
ameba_set(d_DEP_RTK_COEX_API ON)
ameba_set(d_DEP_WPAN ON)

# # - Dependencies defined in make/dependency_mcu.cmake
ameba_set_if(CONFIG_BT d_DEP_BLUETOOTH ON p_ELSE OFF)
# ameba_set(d_DEP_BOOTLOADER ON)
ameba_set(d_DEP_CHIPINFO ON)
# ameba_set(d_DEP_CRASHDUMP ON)
ameba_set(d_DEP_FREERTOS ON)
ameba_set(d_DEP_PMC ON)
ameba_set(d_DEP_USER_PROJECT ON)
if (CONFIG_VMIN_TEST)
    ameba_set(d_DEP_VMIN_TEST ON)
endif()
##################################
if(CONFIG_GUI_EN)
    set(d_DEP_PNG ON)
    set(d_DEP_ZLIB ON)
    set(d_DEP_JPEG ON)
endif()

ameba_set_if(d_DEP_WIFI d_DEP_MISC ON)
ameba_set_if(d_DEP_FREERTOS d_DEP_MISC ON)
ameba_set_if(d_DEP_MISC d_DEP_LWIP ON)

# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #

# +++++++ Define MCU's custom compile and link options ++++++ #
# * Global common options in cmake/common/*_options.cmake and cmake/${mcu}/*_options.cmake

ameba_set(d_CUSTOM_MCU_COMPILE_DEFINES)
ameba_set(d_CUSTOM_MCU_COMPILE_C_OPTIONS)
ameba_set(d_CUSTOM_MCU_COMPILE_CPP_OPTIONS)
ameba_set(d_CUSTOM_MCU_COMPILE_ASM_OPTIONS)

ameba_set(d_CUSTOM_MCU_LINK_OPTIONS)
# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #


# +++++++++++++ Impl MCU's custom config macro ++++++++++++++ #
# * NOTE: This macro is called by ameba_mcu_project_config() automatically

macro(ameba_mcu_project_custom_config)
    if (NOT d_MCU_PROJECT_INITIALIZED)
        ameba_fatal("Call rs_mcu_project_create and ameba_mcu_project_init first")
    endif()

    # if(CONFIG_${d_PLATFORM_TYPE_UPPER}_B_CUT)
    #     ameba_set(ROM_LIB_DIR ${d_SDK_LIB_DIR}/${d_PLATFORM_TYPE}_rom_bcut)
    #     # ameba_set(d_FLOADER_BIN_DIR ${d_SDK_GNU_UTILITY_DIR}/image_tool_flashloader/${d_PLATFORM_TYPE}_acut)
    #     ameba_set(FLOADER_BIN_DIR ${d_SDK_GNU_UTILITY_DIR}/image_tool_flashloader/${d_PLATFORM_TYPE}_acut)
    # else()
    #     ameba_fatal("CONFIG_${d_PLATFORM_TYPE_UPPER}_B_CUT not defined as TRUE")
    # endif()
endmacro()
# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #
