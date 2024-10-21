set(BT_DIR ${BASEDIR}/component/bluetooth)
set(SPP_DIR ${BT_DIR}/example/bt_spp)

set(
    MODULE_IFLAGS
    ${SPP_DIR}
)

set(
	CSRC
	${SPP_DIR}/spp.c
)

target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
