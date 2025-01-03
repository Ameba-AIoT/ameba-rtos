# list(
	# APPEND MODULE_C_DEFINES
	# ZB_ED_ROLE=1
# )

include(../../zcp_ed.cmake)
include(pro_r22_ed_test.cmake)

ameba_app_library(zboss_zcp_test_default_ed)
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_DEFINES})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})