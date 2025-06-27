#Note: Previously defined variables cannot be used directly in this file
#unless passed through -D
cmake_minimum_required(VERSION 3.20.0)
include(${c_CMAKE_FILES_DIR}/axf2bin.cmake)
include(${c_CMAKE_FILES_DIR}/global_define.cmake)
include(${c_CMAKE_FILES_DIR}/utility.cmake)
import_kconfig("CONFIG" ${c_MCU_KCONFIG_FILE})

ameba_execute_process(
    COMMAND ${CMAKE_OBJCOPY} -j .ram_image2.entry -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/entry.bin
    COMMAND ${CMAKE_OBJCOPY} -j .sram_image2.text.data -j .ram_image2_fill -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/sram_2.bin
)

file(READ ${c_SDK_IMAGE_TARGET_DIR}/target_img2.map content)
string(REPLACE "\n" ";" lines ${content})
foreach(_line ${lines})
    string(REGEX MATCH "^.*__exidx_end" result ${_line})
    if(result)
        break()
    endif()
endforeach()
string(REPLACE " " ";" match_line "${result}")
list(GET match_line 0 ARMExAddr)

if(0x${ARMExAddr} GREATER 0x60000000)
    ameba_execute_process(
        COMMAND ${CMAKE_OBJCOPY} -j .psram_image2.text.data -j .ARM.extab -j .ARM.exidx -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/psram_2.bin
        COMMAND ${CMAKE_OBJCOPY} -j .xip_image2.text -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/xip_image2.bin
    )
elseif(0x${ARMExAddr} LESS 0x20000000)
    ameba_execute_process(
        COMMAND ${CMAKE_OBJCOPY} -j .psram_image2.text.data -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/psram_2.bin
        COMMAND ${CMAKE_OBJCOPY} -j .xip_image2.text -j .ARM.extab -j .ARM.exidx -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/xip_image2.bin
    )
else()
    ameba_execute_process(
        COMMAND ${CMAKE_OBJCOPY} -j .psram_image2.text.data -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/psram_2.bin
        COMMAND ${CMAKE_OBJCOPY} -j .xip_image2.text -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/xip_image2.bin
    )
endif()

ameba_execute_process(
    COMMAND ${CMAKE_OBJCOPY} -j .ram_retention.text -j .ram_retention.entry -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/ram_retention.bin
)

if(CONFIG_BT_COEXIST AND CONFIG_COEXIST_DEV)
ameba_execute_process(
    COMMAND ${CMAKE_OBJCOPY} -j .coex_trace.text -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/COEX.trace
)
endif()

message( "========== Image manipulating start ==========")

ameba_axf2bin_pad(${c_SDK_IMAGE_TARGET_DIR}/xip_image2.bin 32)
ameba_axf2bin_pad(${c_SDK_IMAGE_TARGET_DIR}/sram_2.bin 32)
ameba_axf2bin_pad(${c_SDK_IMAGE_TARGET_DIR}/psram_2.bin 32)
ameba_axf2bin_pad(${c_SDK_IMAGE_TARGET_DIR}/entry.bin 32)
ameba_axf2bin_prepend_head(
    ${c_SDK_IMAGE_TARGET_DIR}/xip_image2_prepend.bin
    ${c_SDK_IMAGE_TARGET_DIR}/xip_image2.bin
    __flash_text_start__
    ${c_SDK_IMAGE_TARGET_DIR}/target_img2.map
)
ameba_axf2bin_prepend_head(
    ${c_SDK_IMAGE_TARGET_DIR}/sram_2_prepend.bin
    ${c_SDK_IMAGE_TARGET_DIR}/sram_2.bin
    __sram_image2_start__
    ${c_SDK_IMAGE_TARGET_DIR}/target_img2.map
)
ameba_axf2bin_prepend_head(
    ${c_SDK_IMAGE_TARGET_DIR}/psram_2_prepend.bin
    ${c_SDK_IMAGE_TARGET_DIR}/psram_2.bin
    __psram_image2_start__
    ${c_SDK_IMAGE_TARGET_DIR}/target_img2.map
)
ameba_axf2bin_prepend_head(
    ${c_SDK_IMAGE_TARGET_DIR}/entry_prepend.bin
    ${c_SDK_IMAGE_TARGET_DIR}/entry.bin
    __img2_entry_start__
    ${c_SDK_IMAGE_TARGET_DIR}/target_img2.map
)

ameba_execute_process(
    COMMAND ${CMAKE_COMMAND} -E cat ${c_SDK_IMAGE_TARGET_DIR}/xip_image2_prepend.bin ${c_SDK_IMAGE_TARGET_DIR}/sram_2_prepend.bin ${c_SDK_IMAGE_TARGET_DIR}/psram_2_prepend.bin ${c_SDK_IMAGE_TARGET_DIR}/entry_prepend.bin
    OUTPUT_FILE ${c_SDK_IMAGE_TARGET_DIR}/${c_MCU_PROJECT_NAME}_image2_all.bin
)
ameba_axf2bin_pad(${c_SDK_IMAGE_TARGET_DIR}/${c_MCU_PROJECT_NAME}_image2_all.bin 4096)

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

