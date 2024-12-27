list(
    APPEND MODULE_C_DEFINES
	# CONFIG_ZB_TRACE_LOG_LEVEL=4
	# CONFIG_ZB_TRACE_MASK=0x8000
	ZB_MULTI_TEST=1
)

list(
	APPEND MODULE_IFLAGS
	${ZB_STACK_DIR}/tests/mac_certification/testbin
)

set(CSRC)

list(
    APPEND CSRC 
	${ZB_STACK_DIR}/tests/mac_certification/testbin/zb_multi_mac_test_main.c
	${ZB_STACK_DIR}/tests/mac_certification/testbin/zb_console_monitor.c
	${ZB_STACK_DIR}/tests/mac_certification/testbin/zb_command_parser.c
	${ZB_STACK_DIR}/tests/mac_certification/common/zb_cert_test_globals.c
)

include(../zboss_common.cmake)
include(../zboss_mac.cmake)
include(mac_test.cmake)

ameba_app_library(zboss_mac_test)
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_DEFINES})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})