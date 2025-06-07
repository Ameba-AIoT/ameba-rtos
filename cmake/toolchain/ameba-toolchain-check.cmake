include(${CMAKE_CURRENT_LIST_DIR}/ameba-toolchain-common.cmake)

if(NOT TOOLCHAIN_DIR)
	if (EXISTS ${SDK_TOOLCHAIN})
		# message("ToolChain Had Existed")
	else()
		message("Download Toolchain And Extract To ${TOOLCHAINDIR}. Only Download And Extract Once")
		execute_process(
			COMMAND ${CMAKE_COMMAND} -E make_directory ${TOOLCHAINDIR}
			RESULT_VARIABLE ret
			OUTPUT_VARIABLE stdoutput
			)
		if(ret)
			message(FATAL_ERROR "${stdoutput} Create Toolchain Dir Failed. May Not Have Permission!")
		else()
			message("Create Toolchain Dir ${TOOLCHAINDIR} Success")
		endif()

		if (EXISTS ${TOOLCHAINDIR}/${TOOLCHAINNAME})
			message("${TOOLCHAINNAME} Had Existed")
		else()
			message("Download ${TOOLCHAINNAME} ...")
			execute_process(
				COMMAND wget -P ${TOOLCHAINDIR} ${TOOLCHAINURL}/${TOOLCHAINNAME}
				RESULT_VARIABLE ret
				OUTPUT_VARIABLE stdoutput
			)
			if(ret)
				message(FATAL_ERROR "Download Failed. Please Check If Wget Is Installed And Network Connection Is Accessible!")
			else()
				message("Download ${TOOLCHAINNAME} Success")
			endif()
		endif()

		message("unzip ${TOOLCHAINNAME} ...")

		if (${CMAKE_HOST_SYSTEM_NAME} STREQUAL Linux)
			execute_process(
				COMMAND tar -jxf ${TOOLCHAINDIR}/${TOOLCHAINNAME} -C ${TOOLCHAINDIR}
				RESULT_VARIABLE ret
			)
		else()
			execute_process(
				COMMAND 7z x ${TOOLCHAINDIR}/${TOOLCHAINNAME} -o${TOOLCHAINDIR}
				RESULT_VARIABLE ret
			)
		endif()

		if(ret)
			message(FATAL_ERROR "Unzip Failed. Please unzip ${TOOLCHAINNAME} manually. If still failed, delete ${TOOLCHAINNAME} Manually and Try Again!")
		else()
			message("Unzip ${TOOLCHAINNAME} Success")
		endif()
		execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${TOOLCHAINDIR}/${ToolChainVerMajor}-${ToolChainVerMinor})
		execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${TOOLCHAINDIR}/${ToolChainVerMajor} ${TOOLCHAINDIR}/${ToolChainVerMajor}-${ToolChainVerMinor})
		execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory ${TOOLCHAINDIR}/${ToolChainVerMajor})
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
	string(FIND "${stdoutput}" "${ToolChainVerMinor}" ISMATCH)
	if(ISMATCH GREATER_EQUAL 0)
		message("Toolchain Version Matched")
	else()
		message(SEND_ERROR "Current Toolchain Version Mismatched! Please delete current toolchain /${ToolChainVerMajor} in ${TOOLCHAINDIR} and rebuild. The Latest ToolChain ${TOOLCHAINNAME} will be installed automatically during the project building")
	endif()
endif()

#CHECK git
find_package(Git)
if(Git_FOUND)
	message("Git found: ${GIT_EXECUTABLE}")
endif()

#CHECK python
find_package(Python3)
if(Python3_FOUND)
	message("Python3 found: ${Python3_EXECUTABLE}")
else()
	message(SEND_ERROR "Python check failed")
endif()
