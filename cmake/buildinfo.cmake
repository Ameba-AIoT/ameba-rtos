execute_process(
		COMMAND
			whoami
		TIMEOUT
			1
		OUTPUT_VARIABLE
			_user_name
		OUTPUT_STRIP_TRAILING_WHITESPACE
)
cmake_host_system_information(RESULT _host_name QUERY HOSTNAME)
cmake_host_system_information(RESULT _fqdn QUERY FQDN)

string(TIMESTAMP _configuration_time "%Y-%m-%d %H:%M:%S")
string(TIMESTAMP _configuration_date "%Y-%m-%d")

if (CONFIG_TOOLCHAIN_ARM_GCC)
	set(_compiler_name "ARM_GCC")
else()
	get_filename_component(_compiler_name ${CMAKE_C_COMPILER} NAME)
endif()

configure_file(${CMAKE_FILES_DIR}/build_info.h.in ${PROJECTDIR}/inc/build_info.h @ONLY)