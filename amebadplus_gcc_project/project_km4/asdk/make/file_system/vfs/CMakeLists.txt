set(DIR ${BASEDIR}/component/file_system/vfs)

set(
    MODULE_IFLAGS
    ${BASEDIR}/component/sdio/sd_host/inc
)

set(
    CSRC 
    ${DIR}/vfs_littlefs.c
    ${DIR}/vfs_wrap.c
    ${DIR}/vfs.c
)

if(CONFIG_VFS_FATFS_INCLUDED)
    list(
        APPEND 
        CSRC 
        ${DIR}/vfs_fatfs.c
        ${DIR}/vfs_fatfs_flash.c
        ${DIR}/vfs_fatfs_flash_api.c
    )
endif()


ameba_internal_library(vfs)
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
