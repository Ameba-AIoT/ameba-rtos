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
# set(v_MBEDTLS_VER 3.6.2)
set(v_AMEBA_LOG_LEVEL ${c_AMEBA_LOG_LEVEL_INFO})
set(c_VERBOSE_MAKEFILE FALSE)
###############################################################################
ameba_soc_project_create(amebalite)

#WARNING: project MUST be called after ameba_soc_project_create()
project(${c_SOC_TYPE} LANGUAGES C CXX ASM)

ameba_soc_project_check()
###############################################################################

###############################################################################
#OPTIMIZE: Used in bin-merge process, remove theme if no need
set(KM4_PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/project_km4)
set(KR4_PROJECT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/project_kr4)
###############################################################################

###############################################################################
# Add sub projects
add_subdirectory(project_kr4)
add_subdirectory(project_km4)
###############################################################################

###############################################################################
# Exit project
ameba_soc_project_exit()
###############################################################################