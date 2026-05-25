#Note: Previously defined variables cannot be used directly in this file
#unless passed through -D
cmake_minimum_required(VERSION 3.20.0)
include(${c_CMAKE_FILES_DIR}/utility.cmake)
include(${c_CMAKE_FILES_DIR}/global_define.cmake)
import_kconfig("CONFIG" ${c_MCU_KCONFIG_FILE})

message( "========== Image app generate start ==========")

set(app_full_path ${c_IMAGE_OUTPUT_DIR}/${c_APP_BINARY_NAME})
ameba_modify_file_path(${app_full_path} app_ns_full_path p_SUFFIX _ns)
ameba_modify_file_path(${c_APP_BINARY_NAME} c_APP_BINARY_COMPRESS_NAME p_SUFFIX _compress)

if (CONFIG_WHC_INTF_SDIO)
    ameba_execute_process(COMMAND ${CMAKE_COMMAND} -E rename ${FINAL_IMAGE_DIR}/km4tz_fullmac_img_1.bin ${FINAL_IMAGE_DIR}/RTL8720F_FW_1.bin)
    ameba_execute_process(COMMAND ${CMAKE_COMMAND} -E rename ${FINAL_IMAGE_DIR}/km4tz_fullmac_img_2.bin ${FINAL_IMAGE_DIR}/RTL8720F_FW_2.bin)
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

ameba_execute_process(
    COMMAND ${CMAKE_COMMAND} -E copy ${app_full_path} ${FINAL_IMAGE_DIR}
)

if(CONFIG_SOLO)
    set(ota_image ${c_IMAGE_OUTPUT_DIR}/mcu_ota.bin)
else()
    set(ota_image ${c_IMAGE_OUTPUT_DIR}/ota_all.bin)
endif()

if(CONFIG_COMPRESS_OTA_IMG)
    ameba_modify_file_path(${app_full_path} app_compress p_SUFFIX _compress)
    ameba_axf2bin_compress(${app_compress} ${app_full_path})
    ameba_axf2bin_ota_prepend_head(${ota_image} ${app_compress})
else()
    ameba_axf2bin_ota_prepend_head(${ota_image} ${app_full_path})
endif()

ameba_execute_process(
    COMMAND ${CMAKE_COMMAND} -E copy ${ota_image} ${FINAL_IMAGE_DIR}
)
message( "========== Image app generate end ==========")
