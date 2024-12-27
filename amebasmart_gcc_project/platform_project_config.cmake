set(d_VERBOSE_MAKEFILE FALSE)

# + Override them depend on your project
# + More variables are in global_define.cmake(starts with v_)

# set(v_ASDK_VER 10.3.1)
# set(v_VSDK_VER 10.3.1)
# set(v_FATFS_VER r0.14b)
# set(v_FREERTOS_VER v10.4.3)
# set(v_LITTLEFS_VER r2.50)
# set(v_LWIP_VER v2.1.2)
set(v_MBEDTLS_VER 3.6.2)


set(AP_PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/project_ap)
set(HP_PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/project_hp)
set(LP_PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/project_lp)

set(KM0_PROJECT_DIR ${LP_PROJECT_DIR})
set(KM4_PROJECT_DIR ${HP_PROJECT_DIR})
set(CA32_PROJECT_DIR ${AP_PROJECT_DIR})


# +++++++ Define MCU's custom compile and link options ++++++ #
# * Global common options in cmake/common/*_options.cmake and cmake/${mcu}/*_options.cmake

ameba_set(d_CUSTOM_PLATFORM_COMPILE_DEFINES)
ameba_set(d_CUSTOM_PLATFORM_COMPILE_C_OPTIONS)
ameba_set(d_CUSTOM_PLATFORM_COMPILE_CPP_OPTIONS)
ameba_set(d_CUSTOM_PLATFORM_COMPILE_ASM_OPTIONS)

ameba_set(d_CUSTOM_PLATFORM_LINK_OPTIONS)

if (CONFIG_MP_INCLUDED)
    ameba_set_if(CONFIG_LINUX_FW_EN d_IMAGE_APP_ALL
        ${d_PLATFORM_PROJECT_DIR}/project_hp/asdk/image_mp/km0_km4_app_mp.bin
        p_ELSE
        ${d_PLATFORM_PROJECT_DIR}/project_hp/asdk/image_mp/km0_km4_ca32_app_mp.bin
    )
else()
    ameba_set_if(CONFIG_LINUX_FW_EN d_IMAGE_APP_ALL
        ${d_PLATFORM_PROJECT_DIR}/project_hp/asdk/image/km0_km4_app.bin
        p_ELSE
        ${d_PLATFORM_PROJECT_DIR}/project_hp/asdk/image/km0_km4_ca32_app.bin
    )
endif()
set(d_IMAGE_OTA_ALL ${d_PLATFORM_PROJECT_DIR}/project_hp/asdk/image/ota_all.bin)

macro(ameba_platform_project_custom_config)

endmacro()

# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #

# set(AP_IMAGEDIR		= $(KM4_BUILDDIR)/asdk/image
# else()
# AP_IMAGEDIR		= $(AP_BUILDDIR)/asdk/image
# endif()