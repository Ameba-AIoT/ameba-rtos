set(DIR ${BASEDIR}/component/file_system/kv)

set( 
    CSRC 
    ${DIR}/kv.c
)

set( 
    MODULE_IFLAGS 
    ${BASEDIR}/component/file_system/littlefs 
    ${BASEDIR}/component/file_system/littlefs/${LITTLEFS_VER} 
)


ameba_internal_library(kv)
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
