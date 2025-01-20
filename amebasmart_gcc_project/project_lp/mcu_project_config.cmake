set(CMAKE_VERBOSE_MAKEFILE ON)

# ++++++++ Override variables depend on your project ++++++++ #
# - Full variables(and there default value) are defined in
#   make/global_define.cmake(starts with v_)

# set(v_ASDK_VER 10.3.1)
# set(v_VSDK_VER 10.3.1)
# set(v_FATFS_VER r0.14b)
set(v_FREERTOS_VER v10.5.1)
# set(v_LITTLEFS_VER r2.50)
# set(v_LWIP_VER v2.1.2)
# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #

# +++++++++++++++ Enable or disable dependency ++++++++++++++ #
# - Dependencies defined in make/dependency_common.cmake
# - Use ameba_set or ameba_set_if

ameba_set(d_DEP_CLINTWOOD ON)
ameba_set(d_DEP_EXAMPLE ON)
# ameba_set(d_DEP_LWIP ON)
# ameba_set(d_DEP_NETWORK ON)
ameba_set(d_DEP_OS ON)
# ameba_set(d_DEP_SOC_COMMON ON)
ameba_set(d_DEP_SSL ON)
# ameba_set(d_DEP_STDLIB ON)
# ameba_set(d_DEP_USB_CDC_ECM ON)

# - Dependencies defined in make/dependency_platform.cmake
ameba_set(d_DEP_APP_MONITOR ON)
# ameba_set(d_DEP_APP_XMODEM ON)
ameba_set(d_DEP_CMSIS ON)
ameba_set(d_DEP_FWLIB ON)
# ameba_set(d_DEP_HAL ON)
ameba_set(d_DEP_MISC ON)
ameba_set(d_DEP_SWLIB ON)
ameba_set(d_DEP_TEST ON)
ameba_set(d_DEP_USRCFG ON)
ameba_set(d_DEP_UTILS ON)
ameba_set(d_DEP_WIFI ON)


# - Dependencies defined in make/dependency_mcu.cmake
ameba_set(d_DEP_BOOTLOADER ON)
ameba_set(d_DEP_CHIPINFO ON)
ameba_set(d_DEP_CRASHDUMP ON)
ameba_set(d_DEP_FREERTOS ON)
ameba_set(d_DEP_PMC ON)
ameba_set(d_DEP_USER_PROJECT ON)
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
    # ameba_fatal("PLEASE implement this macro to config rom and remove this line")

    # Specially, lp's MP image folder is same as non-MP: ${d_MCU_SDK_DIR}/image
    ameba_set(IMAGE_TARGET_FOLDER ${d_MCU_SDK_DIR}/image)

    if(CONFIG_AMEBASMART_A_CUT)
        ameba_set(ROM_LIB_DIR ${d_SDK_LIB_DIR}/${d_PLATFORM_TYPE}_rom_acut)
        # ameba_set(d_FLOADER_BIN_DIR ${d_SDK_GNU_UTILITY_DIR}/image_tool_flashloader/${d_PLATFORM_TYPE}_acut)
        ameba_set(FLOADER_BIN_DIR ${d_SDK_GNU_UTILITY_DIR}/image_tool_flashloader/${d_PLATFORM_TYPE}_acut)
    else()
        ameba_fatal("CONFIG_AMEBASMART_A_CUT not defined as TRUE")
    endif()

    if(CONFIG_LINK_ROM_SYMB)
        ameba_unset(LINK_ROM_LIB)
        ameba_set(LINK_ROM_SYMBOL    ${d_SDK_LD_DIR}/ameba_rom_symbol_acut.ld)
    else()
        ameba_set(LINK_ROM_LIB ${ROM_LIB_DIR}/rom.a)
        ameba_set(LINK_ROM_SYMBOL ${d_SDK_LD_DIR}/ameba_rom_symbol_empty.a)
    endif()

    # ameba_unset(LINK_APP_LIB)
    # ameba_list_append(LINK_ROM_LIB          ${d_SDK_LIB_SOC_DIR}/lib_chipinfo.a)
    # ameba_list_append(LINK_ROM_LIB_NS       ${d_SDK_LIB_SOC_DIR}/lib_chipinfo.a)

    # ameba_unset(LINK_THIRD_APP_LIB)
    # ameba_list_append_if(CONFIG_TRUSTZONE LINK_THIRD_APP_LIB ${d_SDK_IMAGE_TARGET_FOLDER}/cmse_implib.lib)
    # ameba_list_append_if(CONFIG_SPEEX_LIB LINK_THIRD_APP_LIB ${CONFIG_SPEEX_LIB}/lib_speexdsp.a)

endmacro()
# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #
