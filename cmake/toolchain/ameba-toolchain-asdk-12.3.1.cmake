set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
set(ToolChainVerMajor asdk-${ASDK_VER})
# CONFIG_* is not imported yet when this file is included, so use c_SOC_TYPE
if(c_SOC_TYPE STREQUAL "amebasmart")
    set(ToolChainVerMinor 4785) # dynamic reent build, required by SMP FreeRTOS (__DYNAMIC_REENT__)
	set(SecondSource https://github.com/Ameba-AIoT/ameba-toolchain/releases/download/12.3.1_v5/)
else()
    set(ToolChainVerMinor 4600)
	set(SecondSource https://github.com/Ameba-AIoT/ameba-toolchain/releases/download/12.3.1_v4/)
endif()
if(USE_SECOND_SOURCE OR "$ENV{USE_SECOND_SOURCE}")
set(TOOLCHAINURL ${SecondSource})
else()
set(TOOLCHAINURL https://aiot.realmcu.com/download/toolchain)
endif()

set(TOOLCHAINDIR)

if(DEFINED ENV{RTK_TOOLCHAIN_DIR})
    set(TOOLCHAINDIR $ENV{RTK_TOOLCHAIN_DIR})
	message("env toolchain path: ${TOOLCHAINDIR}")
endif()

if(TOOLCHAIN_DIR)
	set(TOOLCHAINDIR ${TOOLCHAIN_DIR})
	message("User defined toolchain path: ${TOOLCHAINDIR}")
endif()

file(TO_CMAKE_PATH "${TOOLCHAINDIR}" TOOLCHAINDIR)

if (${CMAKE_HOST_SYSTEM_NAME} STREQUAL Linux)
	if(NOT TOOLCHAINDIR)
		set(TOOLCHAINDIR "$ENV{HOME}/rtk-toolchain")
		message("Default toolchain path: ${TOOLCHAINDIR}")
	endif()
	set(SDK_TOOLCHAIN ${TOOLCHAINDIR}/${ToolChainVerMajor}-${ToolChainVerMinor}/linux/newlib) # Note: server needs to be updated, ${ToolChainVerMajor}-${ToolChainVerMinor}
	set(TOOLCHAINNAME ${ToolChainVerMajor}-linux-newlib-build-${ToolChainVerMinor}-x86_64.tar.bz2)
elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL Darwin)
	if(NOT TOOLCHAINDIR)
		set(TOOLCHAINDIR "$ENV{HOME}/rtk-toolchain")
		message("Default toolchain path: ${TOOLCHAINDIR}")
	endif()
	set(SDK_TOOLCHAIN ${TOOLCHAINDIR}/${ToolChainVerMajor}-${ToolChainVerMinor}/darwin/newlib)
	set(TOOLCHAINNAME ${ToolChainVerMajor}-darwin-newlib-build-${ToolChainVerMinor}-arm64.tar.bz2)
elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL Windows)
	if(NOT TOOLCHAINDIR)
		set(TOOLCHAINDIR C:/rtk-toolchain)
		message("Default toolchain path: ${TOOLCHAINDIR}")
	endif()
	set(SDK_TOOLCHAIN ${TOOLCHAINDIR}/${ToolChainVerMajor}-${ToolChainVerMinor}/mingw32/newlib) # Note: server needs to be updated, ${ToolChainVerMajor}-${ToolChainVerMinor}
	set(TOOLCHAINNAME ${ToolChainVerMajor}-mingw32-newlib-build-${ToolChainVerMinor}-x86_64.zip)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "") # disable ccache
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "")
else()
	message(FATAL_ERROR "unknown host platform ")
endif()

set(CROSS_COMPILE ${SDK_TOOLCHAIN}/bin/arm-none-eabi-)

include(${CMAKE_CURRENT_LIST_DIR}/ameba-toolchain-check.cmake)
