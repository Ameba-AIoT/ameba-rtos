set(DIR ${SRCDIR})

set(MODULE_IFLAGS ${DIR})

set(CSRC ${DIR}/main.c)

ameba_internal_library(project_src)
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})