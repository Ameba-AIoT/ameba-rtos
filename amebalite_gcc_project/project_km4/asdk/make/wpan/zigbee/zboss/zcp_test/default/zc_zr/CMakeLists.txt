list(
	APPEND MODULE_C_DEFINES
)

include(../../zcp_zc_zr.cmake)
include(pro_r22_zc_zr_test.cmake)

ameba_app_library(zboss_zcp_test_default_zc_zr)
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_DEFINES})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})