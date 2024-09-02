set(DIR ${BASEDIR}/component/file_system/ftl)

target_sources(
    target_img2_${PROJECT_NAME}
    PRIVATE 
    ${DIR}/ftl_nor.c
    ${DIR}/ftl.c
)