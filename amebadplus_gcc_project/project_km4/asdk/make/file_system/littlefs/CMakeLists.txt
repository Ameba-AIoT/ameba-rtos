set(DIR ${BASEDIR}/component/file_system/littlefs)

set(
    MODULE_IFLAGS
    ${BASEDIR}/component/file_system/littlefs
    ${BASEDIR}/component/file_system/littlefs/${LITTLEFS_VER}
    ${BASEDIR}/component/file_system/ftl_common
)

set( 
    CSRC 
    ${DIR}/littlefs_adapter.c
    #${DIR}/lfs_nor_flash_api.c
    ${DIR}/${LITTLEFS_VER}/lfs.c
    ${DIR}/${LITTLEFS_VER}/lfs_util.c
)



ameba_internal_library(littlefs) 
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})

