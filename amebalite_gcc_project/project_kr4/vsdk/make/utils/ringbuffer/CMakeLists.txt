set(DIR  ${BASEDIR}/component/utils/ringbuffer)

set(
    CSRC
    ${DIR}/ringbuffer.c
)

ameba_internal_library(utils_ringbuffer)
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
