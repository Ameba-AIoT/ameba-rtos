if (WIN32)
    set(CMAKE_SYSTEM_NAME Generic)
endif()
cmake_minimum_required(VERSION 3.20.0)

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/ameba.cmake)

###############################################################################
#NOTE: Configures, these parameter name is fixed and cannot be changed
# set(v_ASDK_VER 10.3.1)
# set(v_VSDK_VER 10.3.1)
# set(v_FATFS_VER r0.14b)
# set(v_FREERTOS_VER v10.4.3)
# set(v_LITTLEFS_VER r2.50)
# set(v_LWIP_VER v2.1.2)
set(v_MBEDTLS_VER 3.6.2)
set(v_AMEBA_LOG_LEVEL ${c_AMEBA_LOG_LEVEL_INFO})
set(c_VERBOSE_MAKEFILE FALSE)
###############################################################################
ameba_soc_project_create(amebasmart p_OUTPUT_MCU_PROJECT hp)

#WARNING: project MUST be called after ameba_soc_project_create()
project(${c_SOC_TYPE} LANGUAGES C CXX ASM)

ameba_soc_project_check()
###############################################################################
#OPTIMIZE: Used in bin-merge process, remove theme if no need
set(AP_PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/project_ap)
set(HP_PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/project_hp)
set(LP_PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/project_lp)
###############################################################################

###############################################################################
# Add sub projects
ameba_add_subdirectory(project_ap)
ameba_add_subdirectory(project_hp)
ameba_add_subdirectory(project_lp)
###############################################################################

###############################################################################
# Package final firmware
if(CONFIG_LINUX_FW_EN)
    ameba_set_if(CONFIG_MP_INCLUDED app_name km0_km4_app_mp.bin p_ELSE km0_km4_app.bin)
    ameba_firmware_package(
        ${app_name}     #Output app name, referred by c_APP_BINARY_NAME in postbuild.cmake
        p_MCU_PROJECTS  #MCU project list, notice the ORDER is important which determines the image2/image3 order in final binary file
            lp
            hp
    )
else()
    ameba_set_if(CONFIG_MP_INCLUDED app_name km0_km4_ca32_app_mp.bin p_ELSE km0_km4_ca32_app.bin)
    ameba_firmware_package(
        ${app_name}     #Output app name, referred by c_APP_BINARY_NAME in postbuild.cmake
        p_MCU_PROJECTS  #MCU project list, notice the ORDER is important which determines the image2/image3 order in final binary file
            lp
            hp
            ap
    )
endif()
###############################################################################

###############################################################################
# Exit project
ameba_soc_project_exit()
###############################################################################