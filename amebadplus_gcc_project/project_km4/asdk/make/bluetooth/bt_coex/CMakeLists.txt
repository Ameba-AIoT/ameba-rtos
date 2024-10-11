set(DIR ${BASEDIR}/component/bluetooth)

set(CSRC ${DIR}/rtk_coex/rtk_coex.c)

target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
