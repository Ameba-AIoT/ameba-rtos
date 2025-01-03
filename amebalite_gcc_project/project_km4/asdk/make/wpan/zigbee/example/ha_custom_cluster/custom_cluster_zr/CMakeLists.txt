set(ZB_EXAMPLE_DIR ${BASEDIR}/component/wpan/zigbee/example/ha_custom_cluster/custom_cluster_zr)

list(
	APPEND MODULE_IFLAGS
    ${ZB_EXAMPLE_DIR}/
)

set(CSRC)

list(
	APPEND CSRC 
	${ZB_EXAMPLE_DIR}/example.c
)

ameba_internal_library(zb_custom_cluster_zr)
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_DEFINES})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})