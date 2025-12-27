if (WIN32)
    set(CMAKE_SYSTEM_NAME Generic)
endif()
cmake_minimum_required(VERSION 3.20.0)

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/ameba.cmake)

###############################################################################
#NOTE: Configures, these parameter name is fixed and cannot be changed
set(v_ASDK_VER 10.3.1)
set(v_FATFS_VER r0.14b)
set(v_FREERTOS_VER v10.4.3)
set(v_LITTLEFS_VER r2.50)
set(v_LWIP_VER v2.1.2)
set(v_MBEDTLS_VER 3.6.2)
set(v_AMEBA_LOG_LEVEL ${c_AMEBA_LOG_LEVEL_INFO})
set(c_VERBOSE_MAKEFILE FALSE)
###############################################################################
ameba_soc_project_create(amebadplus p_OUTPUT_MCU_PROJECT km4)

#WARNING: project MUST be called after ameba_soc_project_create()
project(${c_SOC_TYPE} LANGUAGES C CXX ASM)

ameba_soc_project_check()
###############################################################################

###############################################################################
#OPTIMIZE: Used in bin-merge process, remove theme if no need
set(KM0_PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/project_km0)
set(KM4_PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/project_km4)
###############################################################################

###############################################################################
# Add sub projects
add_subdirectory(project_km4)
add_subdirectory(project_km0)
###############################################################################

###############################################################################
# Package final firmware
#NOTE: import_kconfig is used for ameba_set_if... later
configure_file(${c_MENUCONFIG_DIR}/.config ${CMAKE_CURRENT_BINARY_DIR}/.config COPYONLY)
import_kconfig("CONFIG" ${CMAKE_CURRENT_BINARY_DIR}/.config)

ameba_set_if(CONFIG_MP_INCLUDED app_name km0_km4_app_mp.bin p_ELSE km0_km4_app.bin)
ameba_firmware_package(
    ${app_name}         #Output app name, referred by c_APP_BINARY_NAME in ${c_POST_BUILD_SCRIPT}
    p_MCU_PROJECTS      #MCU project list, notice the ORDER is important which determines the image2/image3 order in final binary file
        km0 #km0's image2 is in front of km4's image2/image3
        km4
)
###############################################################################

###############################################################################
# Exit project
ameba_soc_project_exit()
###############################################################################
