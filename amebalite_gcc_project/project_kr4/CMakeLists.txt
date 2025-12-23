###############################################################################
#NOTE: Configures, these parameter name is fixed and cannot be changed

set(c_VERBOSE_MAKEFILE FALSE)
###############################################################################

ameba_mcu_project_create(kr4 kr4)
ameba_mcu_project_check()

if(CONFIG_MP_SHRINK)
    if(CONFIG_WHC_HOST)
        message(FATAL_ERROR "Setting KR4 as AP is NOT ALLOWED in SHRINK MP.")
    endif()
endif()
if(CONFIG_WHC_HOST)
    set(AP_IMAGEDIR ${KM4_PROJECT_DIR}/asdk/image)
else()
    set(AP_IMAGEDIR ${KR4_PROJECT_DIR}/vsdk/image)
endif()
###############################################################################

set(DAILY_BUILD 0 CACHE STRING "code analysis argument")
message ("DAILY_BUILD = " ${DAILY_BUILD})
ameba_add_subdirectories(${c_SDK_NAME})