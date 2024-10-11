set(THREADDIR ${BASEDIR}/component/wpan/openthread)
set(RTK_OT_SUPPORT ${BASEDIR}/component/wpan/openthread/rtk_ot_support)
set(RTK_OT_BR ${BASEDIR}/component/wpan/openthread/rtk_otbr)

set(
    MODULE_C_DEFINES
	CONFIG_802154_THREAD_RADIO_RCP_EN=1
	OPENTHREAD_CONFIG_CORE_USER_CONFIG_HEADER_ENABLE=1
)

set(
    MODULE_IFLAGS
	${BASEDIR}/component/soc/amebadplus/fwlib/include/
    ${BASEDIR}/component/component/os/os_dep/
	${RTK_OT_BR}/
	${THREADDIR}/config/
    ${THREADDIR}/openthread/src/
    ${THREADDIR}/openthread/src/core/
    ${THREADDIR}/openthread/include/
    ${THREADDIR}/openthread/examples/
    ${THREADDIR}/openthread/examples/platforms/
    ${THREADDIR}/openthread/examples/platforms/utils/

    ${THREADDIR}/openthread/cli/	
)

if(CONFIG_802154_THREAD_CONCURRENT_EN)
 	list(
		APPEND MODULE_IFLAGS
		${BASEDIR}/component/rtk_coex/rtk_coex_ext_chip/rtk_coex_ext_slave/
	)
endif()

if(CONFIG_802154_THREAD_BORDER_ROUTER_EN)
    add_subdirectory(otbr)
endif()

add_subdirectory(ot_support)






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
option(OT_RCP "enable RCP" OFF)
option(BUILD_TESTING "BUILD_TESTING" OFF)
# option(OT_COMPILE_WARNING_AS_ERROR "whether to include -Werror -pedantic-errors with gcc-compatible compilers" ON)

list(
	APPEND
    MODULE_C_DEFINES
	OPENTHREAD_ENABLE_NCP_SPINEL_ENCRYPTER=0
	OPENTHREAD_UART_RTT_ENABLE=0
	OPENTHREAD_PLATFORM_POSIX=0
)

if(CONFIG_802154_THREAD_BORDER_ROUTER_EN)
	option(OT_FTD "enable FTD" ON)
	option(OT_MTD "enable MTD" OFF)
	set(OT_PACKAGE_NAME "OPENTHREAD_BR" CACHE STRING "OpenThread Package Name")
endif()

if(CONFIG_802154_THREAD_FTD_EN)
	option(OT_FTD "enable FTD" ON)
	option(OT_MTD "enable MTD" OFF)
	set(OT_PACKAGE_NAME "OPENTHREAD_FTD" CACHE STRING "OpenThread Package Name")
endif()

if(CONFIG_802154_THREAD_MTD_EN)
	option(OT_FTD "enable FTD" OFF)
	option(OT_MTD "enable MTD" ON)
	set(OT_PACKAGE_NAME "OPENTHREAD_MTD" CACHE STRING "OpenThread Package Name")
endif()

execute_process(
	WORKING_DIRECTORY ${THREADDIR}/openthread
	# COMMAND ${GIT_EXECUTABLE} checkout ./
	COMMAND ${GIT_EXECUTABLE} apply ./../openthread_tcplp_dup_name_patch.ptach
	COMMAND ${GIT_EXECUTABLE} apply ./../openthread_cmake_testbigendian_patch.patch
	# RESULT_VARIABLE ERROR_CODE
)

# if(ERROR_CODE)
	# message(FATAL_ERROR "git apply ptach error! " ${ERROR_CODE})
# endif()

remove_definitions(-DOT_BIG_ENDIAN)
add_subdirectory(${THREADDIR}/openthread ${CMAKE_CURRENT_BINARY_DIR}/openthread)
target_compile_definitions(ot-config INTERFACE ${MODULE_C_DEFINES})
target_include_directories(ot-config INTERFACE ${MODULE_IFLAGS} ${GLOBAL_IFLAGS})