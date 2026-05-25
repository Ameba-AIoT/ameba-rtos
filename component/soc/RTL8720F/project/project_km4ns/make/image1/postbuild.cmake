#Note: Previously defined variables cannot be used directly in this file
#unless passed through -D
cmake_minimum_required(VERSION 3.20.0)
include(${c_CMAKE_FILES_DIR}/axf2bin.cmake)
include(${c_CMAKE_FILES_DIR}/utility.cmake)
include(${c_CMAKE_FILES_DIR}/global_define.cmake)
import_kconfig("CONFIG" ${c_MCU_KCONFIG_FILE})
set(t_USER_CUSTOM_LOG_PREFIX "KM4NS_LOADER_POSTBUILD")

ameba_execute_process(COMMAND ${CMAKE_OBJCOPY} -j .ram_image1.entry  -j .ram_image1.text -j .ram_image1.data -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_loader.axf ${c_SDK_IMAGE_TARGET_DIR}/ram_1.bin)
ameba_execute_process(COMMAND ${CMAKE_OBJCOPY} -j .xip_image1.text -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_loader.axf ${c_SDK_IMAGE_TARGET_DIR}/xip_boot.bin)
ameba_execute_process(COMMAND ${CMAKE_OBJCOPY} -j .psram_image2.text.data -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_loader.axf ${c_SDK_IMAGE_TARGET_DIR}/psram_1.bin)

if(CONFIG_BT)
    ameba_execute_process(COMMAND ${CMAKE_OBJCOPY} -j .bluetooth_trace.text -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_loader.axf ${c_SDK_IMAGE_TARGET_DIR}/APP.trace)
endif()

if(CONFIG_BT_COEXIST AND CONFIG_COEXIST_DEV)
    ameba_execute_process(COMMAND ${CMAKE_OBJCOPY} -j .coex_trace.text -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_loader.axf ${c_SDK_IMAGE_TARGET_DIR}/COEX.trace)
endif()

message( "========== Image manipulating start ==========")
ameba_axf2bin_pad(${c_SDK_IMAGE_TARGET_DIR}/xip_boot.bin 32)
ameba_axf2bin_pad(${c_SDK_IMAGE_TARGET_DIR}/ram_1.bin 32)
ameba_axf2bin_pad(${c_SDK_IMAGE_TARGET_DIR}/psram_1.bin 32)

ameba_axf2bin_prepend_head(
    ${c_SDK_IMAGE_TARGET_DIR}/ram_1_prepend.bin
    ${c_SDK_IMAGE_TARGET_DIR}/ram_1.bin
    __ram_image1_text_start__
    ${c_SDK_IMAGE_TARGET_DIR}/target_loader.map
    p_FORCE_DEFAULT 1
)
ameba_axf2bin_prepend_head(
    ${c_SDK_IMAGE_TARGET_DIR}/psram_1_prepend.bin
    ${c_SDK_IMAGE_TARGET_DIR}/psram_1.bin
    __psram_image2_start__
    ${c_SDK_IMAGE_TARGET_DIR}/target_loader.map
    p_FORCE_DEFAULT 1
)
ameba_axf2bin_prepend_head(
    ${c_SDK_IMAGE_TARGET_DIR}/xip_boot_prepend.bin
    ${c_SDK_IMAGE_TARGET_DIR}/xip_boot.bin
    __km4ns_boot_text_start__
    ${c_SDK_IMAGE_TARGET_DIR}/target_loader.map
    p_BOOT_INDEX 0x02020202
    p_FORCE_DEFAULT 1
)

ameba_execute_process(
    COMMAND ${CMAKE_COMMAND} -E cat ${c_SDK_IMAGE_TARGET_DIR}/xip_boot_prepend.bin ${c_SDK_IMAGE_TARGET_DIR}/ram_1_prepend.bin ${c_SDK_IMAGE_TARGET_DIR}/psram_1_prepend.bin
    OUTPUT_FILE ${c_SDK_IMAGE_TARGET_DIR}/${c_MCU_PROJECT_NAME}_boot_all.bin
)

message("========== Image manipulating end ==========")

if(NOT CONFIG_AMEBA_RLS)
    message("========== Image analyze start ==========")
# AMEBAL2_TODO
#    ameba_execute_process(p_SHOW_OUTPUT
#        COMMAND ${CODE_ANALYZE_PYTHON} ${ANALYZE_MP_IMG} ${DAILY_BUILD}
#        WORKING_DIRECTORY ${c_SDK_IMAGE_TARGET_DIR}
#    )
#    ameba_execute_process(p_SHOW_OUTPUT
#        COMMAND ${STATIC_ANALYZE_PYTHON} ${DAILY_BUILD} ${c_MCU_PROJECT_DIR}
#        WORKING_DIRECTORY ${c_SDK_IMAGE_TARGET_DIR}/../
#    )
    message("========== Image analyze end ==========")
endif()
