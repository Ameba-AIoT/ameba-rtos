set(BT_DIR ${BASEDIR}/component/bluetooth)
set(RFC_DIR ${BT_DIR}/example/bt_rfc)

set(
    MODULE_IFLAGS
    ${RFC_DIR}
)

set(
	CSRC
	${RFC_DIR}/rfc.c
)

target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
