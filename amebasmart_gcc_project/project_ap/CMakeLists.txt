###############################################################################
#NOTE: Configures, these parameter name is fixed and cannot be changed
#ca32's v_FREERTOS_VER maybe changed in common.cmake by kconfig

set(c_VERBOSE_MAKEFILE FALSE)
###############################################################################

ameba_mcu_project_create(ap ca32)
ameba_mcu_project_check()

###############################################################################
set(DAILY_BUILD 0 CACHE STRING "code analysis argument")
message ("DAILY_BUILD = " ${DAILY_BUILD})
ameba_add_subdirectories_ifnot(CONFIG_LINUX_FW_EN ${c_SDK_NAME})
