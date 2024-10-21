set(
    MODULE_IFLAGS
    ${BASEDIR}/component/audio/base/cutils/interfaces
    ${BASEDIR}/component/audio/base/log/interfaces
    ${BASEDIR}/component/audio/base/osal/osal_c/interfaces
)
set(DIR ${BASEDIR}/component/audio/base/cutils/src)
set(MODULE_C_OPTIONS -D__RTOS__ -Werror -Wno-multichar)
set(
    CSRC
    ${DIR}/parcel.c
    ${DIR}/property.c
    ${DIR}/rtrbtree.c
    ${DIR}/rtintmap.c
    ${DIR}/rtlist.c
)

set_source_files_properties(${CSRC} DIRECTORY ../.. PROPERTIES COMPILE_OPTIONS "${MODULE_C_OPTIONS}")
set_source_files_properties(${CSRC} DIRECTORY ../.. PROPERTIES INCLUDE_DIRECTORIES "${MODULE_IFLAGS}")
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
