if(NOT CMAKE_REFACTOR) # For temporary compatibility
cmake_minimum_required(VERSION 3.20.0)
ameba_set_if(d_VERBOSE_MAKEFILE CMAKE_VERBOSE_MAKEFILE ON p_ELSE OFF)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/ameba.cmake)

ameba_mcu_project_create(hp km4)
ameba_mcu_project_init()
ameba_mcu_project_config() #ameba_mcu_project_custom_config is called internally and automatically

ameba_info("mcu project: ${PROJECT_NAME}, mcu: ${d_MCU_TYPE}, isa: ${d_ISA_TYPE}, sdk: ${d_SDK_NAME}, dir: ${d_MCU_PROJECT_DIR}")
ameba_info("mcu project deps: ${d_MCU_DEPS}")
ameba_info("c_FREERTOS_DIR: ${c_FREERTOS_DIR}")

set(DAILY_BUILD 0 CACHE STRING "code analysis argument")
message ("DAILY_BUILD = " ${DAILY_BUILD})

file(READ ${CMAKE_CURRENT_SOURCE_DIR}/../utils/jlink_script/gdb.py content)
string(REGEX REPLACE "CMAKE_GDB = [^\r\n\t]*" "CMAKE_GDB = \'${CMAKE_GDB}\'" content ${content})
file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/../utils/jlink_script/gdb.py ${content})

add_subdirectory(${d_SDK_NAME})
else() #For temporary compatibility.NOTE:  script below is new version cmake
###############################################################################
#NOTE: Configures, these parameter name is fixed and cannot be changed
# set(v_ASDK_VER 10.3.1)
# set(v_VSDK_VER 10.3.1)
# set(v_FATFS_VER r0.14b)
set(v_FREERTOS_VER v10.5.1)
# set(v_LITTLEFS_VER r2.50)
# set(v_LWIP_VER v2.1.2)

set(c_VERBOSE_MAKEFILE FALSE)
###############################################################################

ameba_mcu_project_create(hp km4)
ameba_mcu_project_check()

###############################################################################

file(READ ${CMAKE_CURRENT_SOURCE_DIR}/../utils/jlink_script/gdb.py content)
string(REGEX REPLACE "CMAKE_GDB = [^\r\n\t]*" "CMAKE_GDB = \'${CMAKE_GDB}\'" content ${content})
file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/../utils/jlink_script/gdb.py ${content})


set(DAILY_BUILD 0 CACHE STRING "code analysis argument")
message ("DAILY_BUILD = " ${DAILY_BUILD})
ameba_add_subdirectories(${c_SDK_NAME})
endif() # For temporary compatibility