set(DIR ${BASEDIR}/component/soc/amebalite/swlib)

set(CSRC ${DIR}/gcc_wrap.c)

ameba_internal_library(os_stdlib_wrap)
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
