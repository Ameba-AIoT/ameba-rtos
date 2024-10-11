set(DIR ${BASEDIR}/component/file_system/ftl)

set(
    CSRC
    ${DIR}/ftl_nor.c
    ${DIR}/ftl.c
)

ameba_internal_library(ftl)
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
