list(
	APPEND MODULE_IFLAGS
	${ZB_STACK_DIR}/tests/multitest_common/
)

list(
    APPEND CSRC 
	${ZB_STACK_DIR}/tests/multitest_common/zb_multitest_main.c
	${ZB_STACK_DIR}/tests/multitest_common/zb_multitest.c
	${ZB_STACK_DIR}/tests/multitest_common/zb_console_monitor.c
	${ZB_STACK_DIR}/tests/multitest_common/zb_command_parser.c
	${ZB_STACK_DIR}/tests/multitest_common/zb_nvram_erase.c
)