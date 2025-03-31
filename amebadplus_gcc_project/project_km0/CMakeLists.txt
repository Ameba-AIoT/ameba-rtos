###############################################################################
#NOTE: Configures, these parameter name is fixed and cannot be changed
set(v_FREERTOS_VER v10.4.3)
set(v_LWIP_VER v2.1.2)
set(v_MBEDTLS_VER 3.6.2)

set(c_VERBOSE_MAKEFILE FALSE)
###############################################################################

ameba_mcu_project_create(km0 km0)
ameba_mcu_project_check()

###############################################################################
set(DAILY_BUILD 0 CACHE STRING "code analysis argument")
message ("DAILY_BUILD = " ${DAILY_BUILD})
ameba_add_subdirectories(${c_SDK_NAME})
