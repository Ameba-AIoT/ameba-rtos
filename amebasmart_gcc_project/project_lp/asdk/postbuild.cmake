#Note: Previously defined variables cannot be used directly in this file
#unless passed through -D
cmake_minimum_required(VERSION 3.20.0)
include(${c_CMAKE_FILES_DIR}/global_define.cmake)
include(${c_CMAKE_FILES_DIR}/axf2bin.cmake)
include(${c_CMAKE_FILES_DIR}/utility.cmake)
import_kconfig("CONFIG" ${c_MCU_KCONFIG_FILE})
ameba_reset_global_define() #NOTE: Some variables like c_MP need to update after import kconfig
set(t_USER_CUSTOM_LOG_PREFIX "LP_POSTBUILD")

ameba_execute_process(
    COMMAND ${CMAKE_OBJCOPY} -j .sram_image2.text.data -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/sram_2.bin
    COMMAND ${CMAKE_OBJCOPY} -j .xip_image2.text -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/xip_image2.bin
    COMMAND ${CMAKE_OBJCOPY} -j .psram_image2.text.data -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/psram_image2.bin
    COMMAND ${CMAKE_OBJCOPY} -j .ram_retention.text -j .ram_retention.entry -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/ram_retention.bin
)

message( "========== Image manipulating start ==========")

ameba_axf2bin_pad(${c_SDK_IMAGE_TARGET_DIR}/sram_2.bin 32)
ameba_axf2bin_pad(${c_SDK_IMAGE_TARGET_DIR}/psram_image2.bin 32)
ameba_axf2bin_pad(${c_SDK_IMAGE_TARGET_DIR}/xip_image2.bin 32)
ameba_axf2bin_prepend_head(
    ${c_SDK_IMAGE_TARGET_DIR}/sram_2_prepend.bin
    ${c_SDK_IMAGE_TARGET_DIR}/sram_2.bin
    __sram_image2_start__
    ${c_SDK_IMAGE_TARGET_DIR}/target_img2.map
)
ameba_axf2bin_prepend_head(
    ${c_SDK_IMAGE_TARGET_DIR}/psram_image2_prepend.bin
    ${c_SDK_IMAGE_TARGET_DIR}/psram_image2.bin
    __psram_image2_start__
    ${c_SDK_IMAGE_TARGET_DIR}/target_img2.map
)
ameba_axf2bin_prepend_head(
    ${c_SDK_IMAGE_TARGET_DIR}/xip_image2_prepend.bin
    ${c_SDK_IMAGE_TARGET_DIR}/xip_image2.bin
    __flash_text_start__
    ${c_SDK_IMAGE_TARGET_DIR}/target_img2.map
)

ameba_execute_process(
    COMMAND ${CMAKE_COMMAND} -E cat ${c_SDK_IMAGE_TARGET_DIR}/xip_image2_prepend.bin ${c_SDK_IMAGE_TARGET_DIR}/sram_2_prepend.bin ${c_SDK_IMAGE_TARGET_DIR}/psram_image2_prepend.bin
    OUTPUT_FILE ${c_SDK_IMAGE_TARGET_DIR}/km0_image2_all.bin
)
ameba_axf2bin_pad(${c_SDK_IMAGE_TARGET_DIR}/km0_image2_all.bin 4096)

message("========== Image manipulating end ==========")

if(NOT CONFIG_AMEBA_RLS)
    message("========== Image analyze start ==========")
    ameba_execute_process(p_SHOW_OUTPUT
        COMMAND ${CODE_ANALYZE_PYTHON} ${ANALYZE_MP_IMG} ${DAILY_BUILD}
        WORKING_DIRECTORY ${c_MCU_PROJECT_DIR}/asdk
    )
    ameba_execute_process(p_SHOW_OUTPUT
        COMMAND ${STATIC_ANALYZE_PYTHON} ${DAILY_BUILD}
        WORKING_DIRECTORY ${c_MCU_PROJECT_DIR}/asdk
    )
    message("========== Image analyze end ==========")
endif()
