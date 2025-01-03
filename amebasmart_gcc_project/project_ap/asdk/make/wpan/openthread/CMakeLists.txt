ameba_target_add(wpan_ot
    p_WRAP_TARGET_NAME
    p_PREFIX lib_
    p_LINK_LIBRARY
        d_PROJ_${d_MCU_PROJECT_NAME_UPPER}_INCLUDE_DIRECTORIES
        d_PROJ_${d_MCU_PROJECT_NAME_UPPER}_LINK_OPTIONS
        src_${d_MCU_PROJECT_NAME}_wpan_ot
    p_APPEND_TO_LIST
        g_${d_MCU_PROJECT_NAME}_IMAGE2_LIBS
    p_DEPENDENTED_BY_TARGETS
        dep_${d_MCU_PROJECT_NAME}_wpan
)

ameba_add_subdirectories_if(CONFIG_802154_THREAD_BORDER_ROUTER_EN otbr)

# for openthread
##########remove warning#########################
set(CMAKE_WARN_DEPRECATED OFF)
set(CMAKE_WARN_ERROR ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
#################################################

set(OT_PLATFORM "external" CACHE STRING "Target platform chosen by the user at configure time")
set(OT_EXTERNAL_MBEDTLS "external" CACHE STRING "Specify external mbedtls library")
option(OT_BUILD_EXECUTABLES "Build executables" OFF)
option(OT_APP_CLI "enable CLI app" ON)
option(OT_APP_NCP "enable NCP app" OFF)
option(OT_APP_RCP "enable RCP app" OFF)
option(BUILD_TESTING "BUILD_TESTING" OFF)
option(OT_NCP_SPI "enable NCP SPI support" OFF)
# option(OT_COMPILE_WARNING_AS_ERROR "whether to include -Werror -pedantic-errors with gcc-compatible compilers" ON)

set(
	OPENTHREAD_C_DEFINES
	OPENTHREAD_UART_RTT_ENABLE=0
	OPENTHREAD_PLATFORM_POSIX=0
	OPENTHREAD_CONFIG_CORE_USER_CONFIG_HEADER_ENABLE=1
)

if(CONFIG_802154_THREAD_BORDER_ROUTER_EN)
	option(OT_FTD "enable FTD" ON)
	option(OT_MTD "enable MTD" OFF)
	option(OT_RCP "enable RCP" OFF)
	set(OT_PACKAGE_NAME "OPENTHREAD_BR" CACHE STRING "OpenThread Package Name")
endif()

if(CONFIG_802154_THREAD_FTD_EN)
	option(OT_FTD "enable FTD" ON)
	option(OT_MTD "enable MTD" OFF)
	option(OT_RCP "enable RCP" OFF)
	set(OT_PACKAGE_NAME "OPENTHREAD_FTD" CACHE STRING "OpenThread Package Name")
endif()

if(CONFIG_802154_THREAD_MTD_EN)
	option(OT_FTD "enable FTD" OFF)
	option(OT_MTD "enable MTD" ON)
	option(OT_RCP "enable RCP" OFF)
	set(OT_PACKAGE_NAME "OPENTHREAD_MTD" CACHE STRING "OpenThread Package Name")
endif()

if(CONFIG_802154_THREAD_RCP_EN)
	option(OT_FTD "enable FTD" OFF)
	option(OT_MTD "enable MTD" OFF)
	option(OT_RCP "enable RCP" ON)
	set(OT_PACKAGE_NAME "OPENTHREAD_RCP" CACHE STRING "OpenThread Package Name")
endif()

execute_process(
	WORKING_DIRECTORY ${c_COMPONENT_DIR}/wpan/openthread/openthread
	# COMMAND ${GIT_EXECUTABLE} checkout ./
	COMMAND ${GIT_EXECUTABLE} apply ./../openthread_tcplp_dup_name_patch.patch
	COMMAND ${GIT_EXECUTABLE} apply ./../openthread_cmake_testbigendian_patch.patch
	# RESULT_VARIABLE ERROR_CODE
)

#add_custom_command(
#    TARGET ${d_CURRENT_TARGET_NAME}
#    PRE_BUILD
#	WORKING_DIRECTORY ${c_COMPONENT_DIR}/wpan/openthread/openthread
#    COMMAND ${GIT_EXECUTABLE} apply ./../openthread_tcplp_dup_name_patch.patch
#	COMMAND ${GIT_EXECUTABLE} apply ./../openthread_cmake_testbigendian_patch.patch
#    COMMENT "Applying Git patch before building target ${d_CURRENT_TARGET_NAME}"
#)

add_subdirectory(${c_COMPONENT_DIR}/wpan/openthread/openthread ${CMAKE_CURRENT_BINARY_DIR}/openthread)
target_compile_definitions(ot-config INTERFACE ${OPENTHREAD_C_DEFINES})
target_link_libraries(ot-config INTERFACE d_PROJ_${d_MCU_PROJECT_NAME_UPPER}_INCLUDE_DIRECTORIES)