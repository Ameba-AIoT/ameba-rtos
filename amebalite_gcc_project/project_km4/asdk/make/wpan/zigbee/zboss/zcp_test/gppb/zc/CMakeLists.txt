set(DIR_GPPB_COOR ${ZB_STACK_DIR}/tests/PRO_R22/certification/GPPB_CERT_COORDINATOR)

list(
	APPEND MODULE_C_DEFINES
)

set(CSRC)

list(
	APPEND CSRC 
	${DIR_GPPB_COOR}/on_off_output_zc.c
)

include(../../zcp_zc_zr.cmake)

ameba_app_library(zboss_zcp_test_gppb_zc)
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_DEFINES})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})