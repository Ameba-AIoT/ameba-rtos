set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

if (${CMAKE_HOST_SYSTEM_NAME} STREQUAL Linux)
	message("current host platform is linux")
	set(EXE_SUFFIX)
	if(TOOLCHAIN_DIR)
		set(TOOLCHAINDIR ${TOOLCHAIN_DIR})
		message("User defined toolchain path: ${TOOLCHAINDIR}")
	else()
		set(TOOLCHAINDIR /opt/rtk-toolchain)
		message("Default toolchain path: ${TOOLCHAINDIR}")
	endif()
	set(ASDK_TOOLCHAIN ${TOOLCHAINDIR}/asdk-${ASDK_VER}/linux/newlib)
	set(TOOLCHAINNAME asdk-12.3.1-linux-newlib-build-4072-x86_64.tar.bz2)
	set(TOOLCHAINURL https://github.com/Ameba-AIoT/ameba-toolchain/releases/download/12.3.1/asdk-12.3.1-linux-newlib-build-4190-x86_64.tar.bz2)
elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL Windows)
	message("current host platform is windows")
	set(EXE_SUFFIX ".exe")
	if(TOOLCHAIN_DIR)
		set(TOOLCHAINDIR ${TOOLCHAIN_DIR})
		message("User defined toolchain path: ${TOOLCHAINDIR}")
	else()
		set(TOOLCHAINDIR C:/rtk-toolchain)
		message("Default toolchain path: ${TOOLCHAINDIR}")
	endif()
	set(ASDK_TOOLCHAIN ${TOOLCHAINDIR}/asdk-${ASDK_VER}/mingw32/newlib)
	set(TOOLCHAINNAME asdk-12.3.1-mingw32-newlib-build-4072-x86_64.zip)
	set(TOOLCHAINURL https://github.com/Ameba-AIoT/ameba-toolchain/releases/download/12.3.1/asdk-12.3.1-mingw32-newlib-build-4190-x86_64.zip)
else()
	message(FATAL_ERROR "unknown host platform ")
endif()

set(TOOLCHAINVER 4190)

set(CROSS_COMPILE ${ASDK_TOOLCHAIN}/bin/arm-none-eabi-)

set(CMAKE_C_COMPILER ${CROSS_COMPILE}gcc${EXE_SUFFIX})
set(CMAKE_CXX_COMPILER ${CROSS_COMPILE}g++${EXE_SUFFIX})
set(CMAKE_ASM_COMPILER ${CROSS_COMPILE}gcc${EXE_SUFFIX})
set(CMAKE_OBJCOPY ${CROSS_COMPILE}objcopy${EXE_SUFFIX})
set(CMAKE_OBJDUMP ${CROSS_COMPILE}objdump${EXE_SUFFIX})
set(CMAKE_SIZE ${CROSS_COMPILE}size${EXE_SUFFIX})
set(CMAKE_STRIP ${CROSS_COMPILE}strip${EXE_SUFFIX})
set(CMAKE_AR ${CROSS_COMPILE}ar${EXE_SUFFIX})
set(CMAKE_AS ${CROSS_COMPILE}as${EXE_SUFFIX})
set(CMAKE_NM ${CROSS_COMPILE}nm${EXE_SUFFIX})
set(CMAKE_LD ${CROSS_COMPILE}ld${EXE_SUFFIX})
set(CMAKE_GDB ${CROSS_COMPILE}gdb${EXE_SUFFIX})
set(CMAKE_READELF ${CROSS_COMPILE}readelf${EXE_SUFFIX})


# set(CMAKE_C_CREATE_STATIC_LIBRARY "<CMAKE_AR> rs <TARGET> <LINK_FLAGS> <OBJECTS>")

if(NOT TOOLCHAIN_DIR)
	if (EXISTS ${ASDK_TOOLCHAIN})
		message("ToolChain Had Existed")
	else()
		message("Download Toolchain And Extract To ${TOOLCHAINDIR}. Only Download And Extract Once")
		execute_process(
			COMMAND ${CMAKE_COMMAND} -E make_directory ${TOOLCHAINDIR}
			RESULT_VARIABLE ret
			OUTPUT_VARIABLE stdoutput
			)
		if(ret)
			message(FATAL_ERROR  "${stdoutput} Create Toolchain Dir Failed. May Not Have Permission. Please Refer To README.MD Or AN900 Chapter 1.3 Installing Toolchain")
		else()
			message("Create Toolchain Dir ${TOOLCHAINDIR} Success")
		endif()

		if (EXISTS ${TOOLCHAINDIR}/${TOOLCHAINNAME})
			message("${TOOLCHAINNAME} Had Existed")
		else()
			message("Download ${TOOLCHAINNAME} ...")
			execute_process(
				COMMAND wget -P ${TOOLCHAINDIR} ${TOOLCHAINURL}
				RESULT_VARIABLE ret
				OUTPUT_VARIABLE stdoutput
			)
			if(ret)
				message(FATAL_ERROR "Download Failed. Please Check If Wget Is Installed And Network Connection Is Accessible Or Refer To AN900 Chapter 1.3")
			else()
				message("Download ${TOOLCHAINNAME} Success")
			endif()
		endif()

		message("unzip ${TOOLCHAINNAME} ...")

		if (${CMAKE_HOST_SYSTEM_NAME} STREQUAL Linux)
			execute_process(COMMAND tar -jxf ${TOOLCHAINDIR}/${TOOLCHAINNAME} -C ${TOOLCHAINDIR})
		else()
			execute_process(COMMAND 7z x ${TOOLCHAINDIR}/${TOOLCHAINNAME} -o${TOOLCHAINDIR})
		endif()
		message("INSTALL SUCCESS")
	endif()
endif()

execute_process(
    COMMAND ${CMAKE_C_COMPILER} -check-license
    OUTPUT_VARIABLE COMMAND_OUTPUT
    ERROR_VARIABLE COMMAND_ERROR
    RESULT_VARIABLE COMMAND_EXIT_CODE
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(${COMMAND_EXIT_CODE} STREQUAL "0")
    if(COMMAND_OUTPUT MATCHES "not found")
        message(FATAL_ERROR "NOT found license, build abort")
    endif()
else()
    message("gcc check-license failed: ${COMMAND_ERROR}")
endif()

#execute_process runs commands while CMake is configuring the project, prior to build system generation
execute_process(
  COMMAND ${CMAKE_C_COMPILER} --version
  RESULT_VARIABLE ret
  OUTPUT_VARIABLE stdoutput
)

if(ret)
	message(SEND_ERROR "Executing the below command failed. Please check toolchain path and permission.
	${CMAKE_C_COMPILER} --version
	${stdoutput}"
	)
else()
	string(FIND "${stdoutput}" "${TOOLCHAINVER}" ISMATCH)
	if(ISMATCH GREATER_EQUAL 0)
		message("Toolchain Version Matched")
	else()
		message(SEND_ERROR "Current Toolchain Version Mismatched! Please delete current toolchain /asdk-${ASDK_VER} in ${TOOLCHAINDIR} and rebuild. The Latest ToolChain ${TOOLCHAINNAME} will be installed automatically during the project building")
	endif()
endif()


#CHECK git
find_package(Git QUIET)
if(Git_FOUND)
	message(STATUS "Git found: ${GIT_EXECUTABLE}")
else()
	message(STATUS "Git not found")
endif()

#CHECK python
find_package(Python3)
if(Python3_FOUND)
	message("Python3 found: ${Python3_EXECUTABLE}")
else()
	message(SEND_ERROR "Python check failed")
endif()
