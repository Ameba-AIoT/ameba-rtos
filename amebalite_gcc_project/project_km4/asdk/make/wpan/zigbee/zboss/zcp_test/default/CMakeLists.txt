list(
    APPEND MODULE_C_DEFINES
	ZB_MULTI_TEST=1
)

list(
	APPEND MODULE_IFLAGS
	${ZB_STACK_DIR}/tests/multitest_common
	${ZB_STACK_DIR}/nwk
	${ZB_STACK_DIR}/tests/PRO_R22/certification/testbin
	${ZB_STACK_DIR}/tests/PRO_R22/certification/common
)

set(CSRC)

include(../../zboss_test_multitest.cmake)

list(
	APPEND CSRC 
	${ZB_STACK_DIR}/tests/multitest_common/zb_test_step_control.c
	${ZB_STACK_DIR}/tests/multitest_common/zb_test_step_storage.c
	${ZB_STACK_DIR}/tests/PRO_R22/certification/common/zb_cert_test_globals.c
	${ZB_STACK_DIR}/tests/PRO_R22/certification/testbin/zb_multitest_r22.c 
)

if(CONFIG_802154_ZIGBEE_ROLE_ZED)
add_subdirectory(ed)
else()
add_subdirectory(zc_zr)
endif()