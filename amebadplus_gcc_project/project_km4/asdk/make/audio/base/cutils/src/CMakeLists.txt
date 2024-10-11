set(
    MODULE_IFLAGS
    ${BASEDIR}/component/audio/base/cutils/interfaces
    ${BASEDIR}/component/audio/base/cutils/src/include
    ${BASEDIR}/component/audio/base/log/interfaces
    ${BASEDIR}/component/audio/base/osal/osal_c/interfaces
)

set(DIR ${BASEDIR}/component/audio/base/cutils/src)

set(MODULE_C_OPTIONS -D__RTOS__ -Wno-multichar -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast)

set(
    CSRC
    ${DIR}/parcel.c
    ${DIR}/rtrbtree.c
    ${DIR}/rtintmap.c
    ${DIR}/rtrbmap.c
    ${DIR}/property.c
    ${DIR}/rtlist.c
    ${DIR}/system_metrics.c
    ${DIR}/system_properties.c
)

set_source_files_properties(${CSRC} DIRECTORY ../.. PROPERTIES COMPILE_OPTIONS "${MODULE_C_OPTIONS}")
set_source_files_properties(${CSRC} DIRECTORY ../.. PROPERTIES INCLUDE_DIRECTORIES "${MODULE_IFLAGS}")
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
