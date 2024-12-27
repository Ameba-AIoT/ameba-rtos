list(
	APPEND CSRC 
	${ZB_STACK_DIR}/common/zb_24bit_math.c
	${ZB_STACK_DIR}/common/zb_48bit_math.c
	${ZB_STACK_DIR}/common/zb_address.c
	${ZB_STACK_DIR}/common/zb_bufpool_mult.c
	${ZB_STACK_DIR}/common/zb_bufpool_mult_storage.c
	${ZB_STACK_DIR}/common/zb_channel_page.c
	${ZB_STACK_DIR}/common/zb_debug.c
	${ZB_STACK_DIR}/common/zb_error_indication.c
	${ZB_STACK_DIR}/common/zb_fsm.c
	${ZB_STACK_DIR}/common/zb_init_common.c
	${ZB_STACK_DIR}/common/zb_init_default.c
	${ZB_STACK_DIR}/common/zb_led_button.c
	${ZB_STACK_DIR}/common/zb_memconfig.c
	${ZB_STACK_DIR}/common/zb_net_trace.c
	${ZB_STACK_DIR}/common/zb_nvram.c
	${ZB_STACK_DIR}/common/zb_nvram_custom_handlers.c
	${ZB_STACK_DIR}/common/zb_osif_single_serial_bridge.c
	${ZB_STACK_DIR}/common/zb_random.c
	${ZB_STACK_DIR}/common/zb_scheduler.c
	${ZB_STACK_DIR}/common/zb_scheduler_init.c
	${ZB_STACK_DIR}/common/zb_serial_trace_bin.c
	${ZB_STACK_DIR}/common/zb_signal_common.c
	${ZB_STACK_DIR}/common/zb_sleep.c
	${ZB_STACK_DIR}/common/zb_time.c
	${ZB_STACK_DIR}/common/zb_trace_common.c
	${ZB_STACK_DIR}/common/zb_verify.c
	${ZB_STACK_DIR}/common/zb_watchdog.c
	
	# ${ZB_STACK_DIR}/common/zb_file_trace.c
	# ${ZB_STACK_DIR}/common/zb_file_trace_bin.c
	# ${ZB_STACK_DIR}/common/zb_globals.c
	# ${ZB_STACK_DIR}/common/zb_nvram_trace.c
)