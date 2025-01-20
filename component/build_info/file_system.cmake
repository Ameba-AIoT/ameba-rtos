if (__INCLUDED_FILE_SYSTEM_BUILD_CMAKE__)
    return()
else()
    set(__INCLUDED_FILE_SYSTEM_BUILD_CMAKE__ TRUE)
endif()

add_library(src_${d_MCU_PROJECT_NAME}_file_system_dct INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_file_system_fatfs INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_file_system_ftl INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_file_system_kv INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_file_system_littlefs INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_file_system_vfs INTERFACE)

#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_file_system_dct p_SCOPE interface
    ${c_CMPT_FILE_SYSTEM_DIR}/dct/crc32.c
    ${c_CMPT_FILE_SYSTEM_DIR}/dct/dct.c
    ${c_CMPT_FILE_SYSTEM_DIR}/dct/dct_util.c
    # ${c_CMPT_FILE_SYSTEM_DIR}/dct/dct_test.c
)
#--------------------------#
ameba_target_include(src_${d_MCU_PROJECT_NAME}_file_system_fatfs p_SCOPE interface
    ${c_COMPONENT_DIR}/usb/host/msc/inc
    ${c_COMPONENT_DIR}/sdio/sd_host/inc
)
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_file_system_fatfs p_SCOPE interface
    ${c_CMPT_FILE_SYSTEM_DIR}/fatfs/disk_if/src/usbdisk.c
    ${c_CMPT_FILE_SYSTEM_DIR}/fatfs/${v_FATFS_VER}/src/ff.c
    ${c_CMPT_FILE_SYSTEM_DIR}/fatfs/${v_FATFS_VER}/src/diskio.c
    ${c_CMPT_FILE_SYSTEM_DIR}/fatfs/${v_FATFS_VER}/src/ffsystem.c
    ${c_CMPT_FILE_SYSTEM_DIR}/fatfs/${v_FATFS_VER}/src/ffunicode.c
)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_file_system_ftl p_SCOPE interface
    ${c_CMPT_FILE_SYSTEM_DIR}/ftl/ftl_nand.c
    ${c_CMPT_FILE_SYSTEM_DIR}/ftl/ftl_nor.c
    ${c_CMPT_FILE_SYSTEM_DIR}/ftl/ftl.c
)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_file_system_kv p_SCOPE interface
    ${c_CMPT_FILE_SYSTEM_DIR}/kv/kv.c
)
#--------------------------#
ameba_target_include(src_${d_MCU_PROJECT_NAME}_file_system_littlefs p_SCOPE interface
    ${c_CMPT_FILE_SYSTEM_DIR}/ftl_common
)
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_file_system_littlefs p_SCOPE interface
    ${c_CMPT_FILE_SYSTEM_DIR}/littlefs/littlefs_adapter.c
    # ${c_CMPT_FILE_SYSTEM_DIR}/littlefs/lfs_nor_flash_api.c
    ${c_CMPT_FILE_SYSTEM_DIR}/littlefs/r2.50/lfs.c
    ${c_CMPT_FILE_SYSTEM_DIR}/littlefs/r2.50/lfs_util.c
    ${c_CMPT_FILE_SYSTEM_DIR}/littlefs/lfs_nand_ftl.c
    ${c_CMPT_FILE_SYSTEM_DIR}/littlefs/lfs_nand_ftl_mfg.c
)
if(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
    if("${d_MCU_PROJECT_NAME}" STREQUAL "ap")
        ameba_target_sources(src_${d_MCU_PROJECT_NAME}_file_system_littlefs p_SCOPE interface
            ${c_CMPT_FILE_SYSTEM_DIR}/littlefs/lfs_spinand.c
        )
    endif()
endif()
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_file_system_vfs p_SCOPE interface
    ${c_CMPT_FILE_SYSTEM_DIR}/vfs/vfs_littlefs.c
    ${c_CMPT_FILE_SYSTEM_DIR}/vfs/vfs_wrap.c
    ${c_CMPT_FILE_SYSTEM_DIR}/vfs/vfs.c
)
ameba_target_sources_if(CONFIG_VFS_FATFS_INCLUDED src_${d_MCU_PROJECT_NAME}_file_system_vfs p_SCOPE interface
    ${c_CMPT_FILE_SYSTEM_DIR}/vfs/vfs_fatfs.c
    ${c_CMPT_FILE_SYSTEM_DIR}/vfs/vfs_fatfs_sdcard_api.c
    ${c_CMPT_FILE_SYSTEM_DIR}/vfs/vfs_fatfs_sdcard.c
    ${c_CMPT_FILE_SYSTEM_DIR}/vfs/vfs_fatfs_flash.c
    ${c_CMPT_FILE_SYSTEM_DIR}/vfs/vfs_fatfs_flash_api.c
)
#--------------------------#
