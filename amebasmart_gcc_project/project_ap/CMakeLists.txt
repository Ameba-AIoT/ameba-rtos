if(NOT CMAKE_REFACTOR) # For temporary compatibility
cmake_minimum_required(VERSION 3.20.0)
set(CMAKE_VERBOSE_MAKEFILE OFF)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/ameba.cmake)

ameba_mcu_project_create(ap ca32)
ameba_mcu_project_init()
ameba_mcu_project_config() #ameba_mcu_project_custom_config is called internally and automatically

ameba_info("mcu project: ${PROJECT_NAME}, mcu: ${d_MCU_TYPE}, isa: ${d_ISA_TYPE}, sdk: ${d_SDK_NAME}, dir: ${d_MCU_PROJECT_DIR}")
ameba_info("mcu project deps: ${d_MCU_DEPS}")
ameba_info("c_FREERTOS_DIR: ${c_FREERTOS_DIR}")

set(DAILY_BUILD 0 CACHE STRING "code analysis argument")
message ("DAILY_BUILD = " ${DAILY_BUILD})
ameba_add_subdirectories_ifnot(CONFIG_LINUX_FW_EN ${d_SDK_NAME})

else() #For temporary compatibility.NOTE:  script below is new version cmake
###############################################################################
#NOTE: Configures, these parameter name is fixed and cannot be changed
# set(v_ASDK_VER 10.3.1)
# set(v_VSDK_VER 10.3.1)
# set(v_FATFS_VER r0.14b)
set(v_FREERTOS_VER v10.2.1)
set(c_FREERTOS_DIR_SUFFIX _smp)
# set(v_LITTLEFS_VER r2.50)
# set(v_LWIP_VER v2.1.2)

set(c_VERBOSE_MAKEFILE FALSE)
###############################################################################

#
ameba_mcu_project_create(ap ca32)
ameba_mcu_project_check()

###############################################################################
set(DAILY_BUILD 0 CACHE STRING "code analysis argument")
message ("DAILY_BUILD = " ${DAILY_BUILD})
ameba_add_subdirectories_ifnot(CONFIG_LINUX_FW_EN ${c_SDK_NAME})
endif() # For temporary compatibility