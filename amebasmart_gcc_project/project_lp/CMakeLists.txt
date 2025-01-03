cmake_minimum_required(VERSION 3.20.0)
ameba_set_if(d_VERBOSE_MAKEFILE CMAKE_VERBOSE_MAKEFILE ON p_ELSE OFF)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/ameba.cmake)

ameba_mcu_project_create(lp km0)
ameba_mcu_project_init()
ameba_mcu_project_config() #ameba_mcu_project_custom_config is called internally and automatically

ameba_info("mcu project: ${PROJECT_NAME}, mcu: ${d_MCU_TYPE}, isa: ${d_ISA_TYPE}, sdk: ${d_SDK_NAME}, dir: ${d_MCU_PROJECT_DIR}")
ameba_info("mcu project deps: ${d_MCU_DEPS}")
ameba_info("c_FREERTOS_DIR: ${c_FREERTOS_DIR}")

set(DAILY_BUILD 0 CACHE STRING "code analysis argument")
message ("DAILY_BUILD = " ${DAILY_BUILD})

add_subdirectory(${d_SDK_NAME})
