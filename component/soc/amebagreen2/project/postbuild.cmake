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
ameba_modify_file_path(${app_full_path} app_tmp_full_path p_SUFFIX _tmp)
ameba_modify_file_path(${app_ns_full_path} app_ns_tmp_full_path p_SUFFIX _tmp)
ameba_modify_file_path(${app_full_path} app_compress p_SUFFIX _compress)
ameba_modify_file_path(${app_ns_full_path} app_compress_ns p_SUFFIX _compress)

if (CONFIG_WHC_INTF_SDIO)
    ameba_execute_process(COMMAND ${CMAKE_COMMAND} -E rename ${FINAL_IMAGE_DIR}/km4tz_fullmac_img_1.bin ${FINAL_IMAGE_DIR}/RTL8851FS_FW_1.bin)
    ameba_execute_process(COMMAND ${CMAKE_COMMAND} -E rename ${FINAL_IMAGE_DIR}/km4tz_fullmac_img_2.bin ${FINAL_IMAGE_DIR}/RTL8851FS_FW_2.bin)
elseif (CONFIG_WHC_INTF_USB)
    ameba_execute_process(COMMAND ${CMAKE_COMMAND} -E rename ${FINAL_IMAGE_DIR}/km4tz_fullmac_img_1.bin ${FINAL_IMAGE_DIR}/RTL8851FU_FW_1.bin)
    ameba_execute_process(COMMAND ${CMAKE_COMMAND} -E rename ${FINAL_IMAGE_DIR}/km4tz_fullmac_img_2.bin ${FINAL_IMAGE_DIR}/RTL8851FU_FW_2.bin)
else()
    ameba_execute_process(COMMAND ${CMAKE_COMMAND} -E remove ${FINAL_IMAGE_DIR}/km4tz_fullmac_img_1.bin)
    ameba_execute_process(COMMAND ${CMAKE_COMMAND} -E remove ${FINAL_IMAGE_DIR}/km4tz_fullmac_img_2.bin)
endif()

if(CONFIG_TRUSTZONE)
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

# Read-only littlefs ROM image: build a blob = 32B PATTERN_ROLFS IMAGE_HEADER +
# littlefs payload, and append it to the (already fw_pack'd) app image, BEFORE
# compression / OTA prepend, so it is carried inside both app.bin and ota_all.bin
# and lands within the app OTA slot. The blob is packed tightly right after the
# app (32B aligned, no 4KB waste); the runtime VFS scans for the header in 32B
# steps and mounts the payload that follows it read-only at "rolfs:".
# The content directory is owned by the application, which registers it via
# ameba_add_rolfs_content() (forwarded here as ROLFS_CONTENT_DIR). When set and it
# exists, it is packed by the generic vfs.py image builder with --rolfs-header,
# which appends the PATTERN_ROLFS IMAGE_HEADER the runtime scanner looks for.
if(CONFIG_LITTLEFS_WITHIN_APP_IMG AND ROLFS_CONTENT_DIR AND EXISTS ${ROLFS_CONTENT_DIR})
    set(rolfs_blob_path ${c_IMAGE_OUTPUT_DIR}/littlefs_rolfs.bin)
    ameba_execute_process(
        COMMAND python ${c_BASEDIR}/tools/image_scripts/vfs.py
            -t LITTLEFS
            --rolfs-header
            -dir ${ROLFS_CONTENT_DIR}
            -out ${rolfs_blob_path}
    )
    ameba_modify_file_path(${app_full_path} app_rolfs_tmp_full_path p_SUFFIX _rolfs_tmp)
    ameba_execute_process(
        COMMAND ${CMAKE_COMMAND} -E rename ${app_full_path} ${app_rolfs_tmp_full_path}
    )
    ameba_axf2bin_pad(${app_rolfs_tmp_full_path} 32)  #32B-align the header, matches the runtime scan step
    ameba_execute_process(
        COMMAND ${CMAKE_COMMAND} -E cat ${app_rolfs_tmp_full_path} ${rolfs_blob_path}
        OUTPUT_FILE ${app_full_path}
    )
    ameba_execute_process(COMMAND ${CMAKE_COMMAND} -E remove ${app_rolfs_tmp_full_path})
    message("========== ROLFS littlefs blob appended to app image ==========")
endif()

if(CONFIG_FATFS_WITHIN_APP_IMG)
    if(EXISTS ${c_SOC_PROJECT_DIR}/fatfs.bin)
        ameba_execute_process(
            COMMAND ${CMAKE_COMMAND} -E rename ${app_full_path} ${app_tmp_full_path}
        )
        ameba_execute_process(
            COMMAND ${CMAKE_COMMAND} -E rename ${app_ns_full_path} ${app_ns_tmp_full_path}
        )
        ameba_axf2bin_pad(${app_tmp_full_path} 4096)
        ameba_axf2bin_pad(${app_ns_tmp_full_path} 4096)
        ameba_axf2bin_prepend_head(
            ${c_SOC_PROJECT_DIR}/fatfs_prepend.bin
            ${c_SOC_PROJECT_DIR}/fatfs.bin
            VFS1_FLASH_BASE_ADDR
           ${c_IMAGE_OUTPUT_DIR}/target_img2.map
        )

        ameba_execute_process(
            COMMAND ${CMAKE_COMMAND} -E cat ${app_tmp_full_path} ${c_SOC_PROJECT_DIR}/fatfs_prepend.bin
            OUTPUT_FILE ${app_full_path}
        )
        ameba_execute_process(
            COMMAND ${CMAKE_COMMAND} -E cat ${app_ns_tmp_full_path} ${c_SOC_PROJECT_DIR}/fatfs_prepend.bin
            OUTPUT_FILE ${app_ns_full_path}
        )
    else()
        message(FATAL_ERROR "Lack of fatfs.bin when enabling CONFIG_FATFS_WITHIN_APP_IMG")
    endif()
endif()

if(CONFIG_COMPRESS_OTA_IMG)
    ameba_axf2bin_compress(${app_compress} ${app_full_path})
    ameba_axf2bin_compress(${app_compress_ns} ${app_ns_full_path})
    ameba_execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${app_compress} ${c_IMAGE_OUTPUT_DIR}/tmp_app.bin)
    ameba_execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${app_compress_ns} ${c_IMAGE_OUTPUT_DIR}/tmp_app_ns.bin)
else()
    ameba_execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${app_full_path} ${c_IMAGE_OUTPUT_DIR}/tmp_app.bin)
    ameba_execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${app_ns_full_path} ${c_IMAGE_OUTPUT_DIR}/tmp_app_ns.bin)
endif()

if(CONFIG_UPGRADE_BOOTLOADER)
    ameba_modify_file_path(${c_IMAGE1_ALL_FILES} boot_ns_full_path p_SUFFIX _ns)
    ameba_axf2bin_ota_prepend_head(${c_IMAGE_OUTPUT_DIR}/ota_all.bin ${c_IMAGE_OUTPUT_DIR}/tmp_app.bin ${c_IMAGE1_ALL_FILES})
    ameba_axf2bin_ota_prepend_head(${c_IMAGE_OUTPUT_DIR}/ota_all_ns.bin ${c_IMAGE_OUTPUT_DIR}/tmp_app_ns.bin ${boot_ns_full_path})
else()
    ameba_axf2bin_ota_prepend_head(${c_IMAGE_OUTPUT_DIR}/ota_all.bin ${c_IMAGE_OUTPUT_DIR}/tmp_app.bin)
    ameba_axf2bin_ota_prepend_head(${c_IMAGE_OUTPUT_DIR}/ota_all_ns.bin ${c_IMAGE_OUTPUT_DIR}/tmp_app_ns.bin)
endif()
ameba_execute_process(COMMAND ${CMAKE_COMMAND} -E remove ${c_IMAGE_OUTPUT_DIR}/tmp_app.bin)
ameba_execute_process(COMMAND ${CMAKE_COMMAND} -E remove ${c_IMAGE_OUTPUT_DIR}/tmp_app_ns.bin)

message( "========== Image app generate end ==========")

ameba_execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${c_IMAGE_OUTPUT_DIR}/ota_all.bin ${FINAL_IMAGE_DIR})
ameba_execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${app_full_path} ${FINAL_IMAGE_DIR})
