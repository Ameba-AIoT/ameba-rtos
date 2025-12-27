###############################################################################
#NOTE: Configures, these parameter name is fixed and cannot be changed

set(c_VERBOSE_MAKEFILE FALSE)
###############################################################################

ameba_mcu_project_create(km4 km4)
ameba_mcu_project_check()
if(CONFIG_AS_INIC_AP)
    set(AP_IMAGEDIR ${KM4_PROJECT_DIR}/asdk/image)
else()
    set(AP_IMAGEDIR ${KR4_PROJECT_DIR}/vsdk/image)
endif()
###############################################################################
file(READ ${CMAKE_CURRENT_SOURCE_DIR}/../utils/jlink_script/gdb.py content)
string(REGEX REPLACE "CMAKE_GDB = [^\r\n\t]*" "CMAKE_GDB = \'${CMAKE_GDB}\'" content ${content})
file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/../utils/jlink_script/gdb.py ${content})

set(DAILY_BUILD 0 CACHE STRING "code analysis argument")
message ("DAILY_BUILD = " ${DAILY_BUILD})
ameba_add_subdirectories(${c_SDK_NAME})