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

if(CONFIG_TRUSTZONE OR CONFIG_TRUSTZONE_FOR_KM4)
    if(CONFIG_DSP_WITHIN_APP_IMG)
        ameba_axf2bin_fw_pack(
            ${app_full_path}
            p_IMAGE2 ${c_IMAGE2_ALL_FILES}
            p_IMAGE3 ${c_IMAGE3_ALL_FILES}
            p_DSP ${c_DSP_FILE}
        )
    else()
        ameba_axf2bin_fw_pack(
            ${app_full_path}
            p_IMAGE2 ${c_IMAGE2_ALL_FILES}
            p_IMAGE3 ${c_IMAGE3_ALL_FILES}
        )
    endif()
else()
    if(CONFIG_DSP_WITHIN_APP_IMG)
        ameba_axf2bin_fw_pack(
            ${app_full_path}
            p_IMAGE2 ${c_IMAGE2_ALL_FILES}
            p_DSP ${c_DSP_FILE}
        )
    else()
        ameba_axf2bin_fw_pack(
            ${app_full_path}
            p_IMAGE2 ${c_IMAGE2_ALL_FILES}
        )
    endif()
endif()

if(CONFIG_FATFS_WITHIN_APP_IMG)
    if(EXISTS ${c_SOC_PROJECT_DIR}/fatfs.bin)
        ameba_execute_process(
            COMMAND ${CMAKE_COMMAND} -E rename ${app_full_path} ${app_tmp_full_path}
            COMMAND ${CMAKE_COMMAND} -E rename${app_ns_full_path} ${app_ns_tmp_full_path}
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
    ameba_axf2bin_ota_prepend_head(${c_IMAGE_OUTPUT_DIR}/ota_all.bin ${c_IMAGE_OUTPUT_DIR}/tmp_app.bin ${c_IMAGE_OUTPUT_DIR}/km4_boot_all.bin)
    ameba_axf2bin_ota_prepend_head(${c_IMAGE_OUTPUT_DIR}/ota_all_ns.bin ${c_IMAGE_OUTPUT_DIR}/tmp_app_ns.bin ${c_IMAGE_OUTPUT_DIR}/km4_boot_all_ns.bin)
else()
    ameba_axf2bin_ota_prepend_head(${c_IMAGE_OUTPUT_DIR}/ota_all.bin ${c_IMAGE_OUTPUT_DIR}/tmp_app.bin)
    ameba_axf2bin_ota_prepend_head(${c_IMAGE_OUTPUT_DIR}/ota_all_ns.bin ${c_IMAGE_OUTPUT_DIR}/tmp_app_ns.bin)
endif()

ameba_execute_process(
    COMMAND ${CMAKE_COMMAND} -E remove ${c_SDK_IMAGE_TARGET_DIR}/tmp_app.bin
    COMMAND ${CMAKE_COMMAND} -E remove ${c_SDK_IMAGE_TARGET_DIR}/tmp_app_ns.bin
)
message("========== Image manipulating end ==========")

ameba_execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${c_IMAGE_OUTPUT_DIR}/ota_all.bin ${FINAL_IMAGE_DIR})
ameba_execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${app_full_path} ${FINAL_IMAGE_DIR})
