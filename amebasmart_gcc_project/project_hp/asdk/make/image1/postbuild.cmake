#Note: Previously defined variables cannot be used directly in this file
#unless passed through -D
cmake_minimum_required(VERSION 3.20.0)
include(${c_CMAKE_FILES_DIR}/axf2bin.cmake)
include(${c_CMAKE_FILES_DIR}/utility.cmake)
include(${c_CMAKE_FILES_DIR}/global_define.cmake)
import_kconfig("CONFIG" ${c_MCU_KCONFIG_FILE})
ameba_reset_global_define() #NOTE: Some variables like c_MP need to update after import kconfig
set(t_USER_CUSTOM_LOG_PREFIX "HP_POSTBUILD")

ameba_execute_process(
    COMMAND ${CMAKE_OBJCOPY} -j .ram_image1.entry  -j .ram_image1.text -j .ram_image1.data -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_loader.axf ${c_SDK_IMAGE_TARGET_DIR}/ram_1.bin
)

ameba_execute_process(
    COMMAND ${CMAKE_OBJCOPY} -j .xip_image1.text -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_loader.axf ${c_SDK_IMAGE_TARGET_DIR}/xip_boot.bin
)

message( "========== Image manipulating start ==========")

ameba_axf2bin_pad(${c_SDK_IMAGE_TARGET_DIR}/ram_1.bin 32)
ameba_axf2bin_prepend_head(
    ${c_SDK_IMAGE_TARGET_DIR}/ram_1_prepend.bin
    ${c_SDK_IMAGE_TARGET_DIR}/ram_1.bin
    __ram_start_table_start__
    ${c_SDK_IMAGE_TARGET_DIR}/target_loader.map
)
if(IMGTOOL_LOADER)
    ameba_axf2bin_fw_pack(
        ${c_SDK_IMAGE_TARGET_DIR}/imgtool_flashloader.bin
        p_IMAGE_IMGTOOL_FLOADER
            ${c_SDK_IMAGE_TARGET_DIR}/ram_1_prepend.bin
    )
else()
    ameba_copy_file(${c_SDK_IMAGE_TARGET_DIR}/ram_1_prepend.bin ${c_SDK_IMAGE_TARGET_DIR}/km4_boot_all.bin)
    ameba_axf2bin_fw_pack(
        ${c_SDK_IMAGE_TARGET_DIR}/km4_boot_all.bin
        p_IMAGE1
            ${c_SDK_IMAGE_TARGET_DIR}/km4_boot_all.bin
    )
endif()

message( "========== Image manipulating end ==========")

if (NOT IMGTOOL_LOADER)
    ameba_execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy ${c_SDK_IMAGE_TARGET_DIR}/km4_boot_all.bin ${FINAL_IMAGE_DIR}
    )
endif()
