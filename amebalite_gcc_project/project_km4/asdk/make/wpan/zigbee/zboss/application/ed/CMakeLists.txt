list(
	APPEND MODULE_C_DEFINES
	ZB_ED_ROLE=1
)

set(CSRC)

include(../../zboss_common.cmake)
include(../../zboss_mac.cmake)
include(../../zboss_nwk.cmake)
include(../../zboss_secur.cmake)
include(../../zboss_aps.cmake)
include(../../zboss_zdo.cmake)
include(../../zboss_comm_bdb.cmake)
include(../../zboss_comm_legacy.cmake)
include(../../zboss_comm_se.cmake)
include(../../zboss_zcl.cmake)
include(../../zboss_zse.cmake)

ameba_app_library(zboss_ed)
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_DEFINES})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})