set(DIR ${BASEDIR}/component/bluetooth)

set(CSRC ${DIR}/rtk_coex/rtk_coex.c)

target_sources(target_img2_${PROJECT_NAME} PRIVATE ${CSRC})