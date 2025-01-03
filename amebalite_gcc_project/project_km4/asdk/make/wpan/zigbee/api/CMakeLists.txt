set(ZB_API_DIR ${BASEDIR}/component/wpan/zigbee/api)

set(CSRC)

list(
	APPEND CSRC 
	${ZB_API_DIR}/zigbee_helpers.c
	${ZB_API_DIR}/zb_error_to_string.c
)

ameba_internal_library(zb_api)
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_DEFINES})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})