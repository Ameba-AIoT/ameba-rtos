#Note: Previously defined variables cannot be used directly in this file
#unless passed through -D
cmake_minimum_required(VERSION 3.20.0)
include(${c_CMAKE_FILES_DIR}/utility.cmake)
include(${c_CMAKE_FILES_DIR}/global_define.cmake)
import_kconfig("CONFIG" ${c_MCU_KCONFIG_FILE})
ameba_reset_global_define() #NOTE: Some variables like c_MP need to update after import kconfig

message( "========== Image app generate start ==========")

set(app_full_path ${c_IMAGE_OUTPUT_DIR}/${c_APP_BINARY_NAME})
ameba_modify_file_path(${app_full_path} app_ns_full_path p_SUFFIX _ns)

if (c_IMAGE3_ALL_FILES)
    ameba_axf2bin_fw_pack(
        ${app_full_path}
        p_IMAGE2 ${c_IMAGE2_ALL_FILES}
        p_IMAGE3 ${c_IMAGE3_ALL_FILES}
    )
else()
    ameba_axf2bin_fw_pack(
        ${app_full_path}
        p_IMAGE2 ${c_IMAGE2_ALL_FILES}
    )
endif()

if(CONFIG_FATFS_WITHIN_APP_IMG)
    if(EXISTS {c_SOC_PROJECT_DIR}/fatfs.bin)
        ameba_axf2bin_pad(${app_full_path} 4096)
        ameba_axf2bin_pad(${app_ns_full_path} 4096)
        ameba_axf2bin_prepend_head(
            {c_SOC_PROJECT_DIR}/fatfs_prepend.bin
            {c_SOC_PROJECT_DIR}/fatfs.bin
            VFS1_FLASH_BASE_ADDR
            ${c_IMAGE_OUTPUT_DIR}/target_img2.map
        )
        ameba_file_append(${app_full_path} {c_SOC_PROJECT_DIR}/fatfs_prepend.bin)
        ameba_file_append(${app_ns_full_path} {c_SOC_PROJECT_DIR}/fatfs_prepend.bin)
    endif()
endif()

if(CONFIG_COMPRESS_OTA_IMG)
    ameba_modify_file_path(${app_full_path} app_compress p_SUFFIX _compress)
    ameba_modify_file_path(${app_ns_full_path} app_ns_compress p_SUFFIX _compress)

    ameba_axf2bin_compress(${app_compress} ${app_full_path})
    ameba_axf2bin_compress(${app_ns_compress} ${app_ns_full_path})
    ameba_copy_file(${app_compress} ${c_IMAGE_OUTPUT_DIR}/tmp_app.bin)
    ameba_copy_file(${app_ns_compress} ${c_IMAGE_OUTPUT_DIR}/tmp_app_ns.bin)
else()
    ameba_copy_file(${app_full_path} ${c_IMAGE_OUTPUT_DIR}/tmp_app.bin)
    ameba_copy_file(${app_ns_full_path} ${c_IMAGE_OUTPUT_DIR}/tmp_app_ns.bin)
endif()

if(CONFIG_UPGRADE_BOOTLOADER)
    ameba_modify_file_path(${c_IMAGE1_ALL_FILES} boot_ns_full_path p_SUFFIX _ns)
    ameba_axf2bin_ota_prepend_head(${c_IMAGE_OUTPUT_DIR}/ota_all.bin ${c_IMAGE_OUTPUT_DIR}/tmp_app.bin ${c_IMAGE1_ALL_FILES})
    ameba_axf2bin_ota_prepend_head(${c_IMAGE_OUTPUT_DIR}/ota_all_ns.bin ${c_IMAGE_OUTPUT_DIR}/tmp_app_ns.bin ${boot_ns_full_path})
else()
    ameba_axf2bin_ota_prepend_head(${c_IMAGE_OUTPUT_DIR}/ota_all.bin ${c_IMAGE_OUTPUT_DIR}/tmp_app.bin)
    ameba_axf2bin_ota_prepend_head(${c_IMAGE_OUTPUT_DIR}/ota_all_ns.bin ${c_IMAGE_OUTPUT_DIR}/tmp_app_ns.bin)
endif()

ameba_remove_file(${c_IMAGE_OUTPUT_DIR}/tmp_app.bin ${c_IMAGE_OUTPUT_DIR}/tmp_app_ns.bin ${c_IMAGE_OUTPUT_DIR}/*prepend.bin)

ameba_copy_file(${app_full_path} ${FINAL_IMAGE_DIR})
ameba_copy_file(${c_IMAGE_OUTPUT_DIR}/ota_all.bin ${FINAL_IMAGE_DIR})

message("========== Image manipulating end ==========")
