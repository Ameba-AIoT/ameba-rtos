#Note: Previously defined variables cannot be used directly in this file
#unless passed through -D
cmake_minimum_required(VERSION 3.20.0)
include(${c_CMAKE_FILES_DIR}/axf2bin.cmake)
include(${c_CMAKE_FILES_DIR}/utility.cmake)
include(${c_CMAKE_FILES_DIR}/global_define.cmake)
import_kconfig("CONFIG" ${c_MCU_KCONFIG_FILE})
ameba_reset_global_define() #NOTE: Some variables like c_MP need to update after import kconfig
set(t_USER_CUSTOM_LOG_PREFIX "KM4TZ_IMG2_POSTBUILD")

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
        COMMAND ${CMAKE_OBJCOPY} -j .sram_image2.text.data -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/sram_2.bin
        COMMAND ${CMAKE_OBJCOPY} -j .xip_image2.text -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/xip_image2.bin
    )
else()
    ameba_execute_process(
        COMMAND ${CMAKE_OBJCOPY} -j .psram_image2.text.data -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/psram_2.bin
        COMMAND ${CMAKE_OBJCOPY} -j .sram_image2.text.data -j .ARM.extab -j .ARM.exidx -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/sram_2.bin
        COMMAND ${CMAKE_OBJCOPY} -j .xip_image2.text -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/xip_image2.bin
    )
endif()

if(CONFIG_BT)
    ameba_execute_process(COMMAND ${CMAKE_OBJCOPY} -j .bluetooth_trace.text -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/APP.trace)
endif()

if(CONFIG_BT_COEXIST AND CONFIG_COEXIST_DEV)
    ameba_execute_process(COMMAND ${CMAKE_OBJCOPY} -j .coex_trace.text -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/COEX.trace)
endif()

message( "========== Image manipulating start ==========")

ameba_axf2bin_pad(${c_SDK_IMAGE_TARGET_DIR}/xip_image2.bin 32)
ameba_axf2bin_pad(${c_SDK_IMAGE_TARGET_DIR}/sram_2.bin 32)
ameba_axf2bin_pad(${c_SDK_IMAGE_TARGET_DIR}/psram_2.bin 32)
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
    ${c_SDK_IMAGE_TARGET_DIR}/xip_image2_prepend.bin
    ${c_SDK_IMAGE_TARGET_DIR}/xip_image2.bin
    __flash_text_start__
    ${c_SDK_IMAGE_TARGET_DIR}/target_img2.map
)

ameba_execute_process(
    COMMAND ${CMAKE_COMMAND} -E cat ${c_SDK_IMAGE_TARGET_DIR}/xip_image2_prepend.bin ${c_SDK_IMAGE_TARGET_DIR}/sram_2_prepend.bin ${c_SDK_IMAGE_TARGET_DIR}/psram_2_prepend.bin
    OUTPUT_FILE ${c_SDK_IMAGE_TARGET_DIR}/${c_MCU_PROJECT_NAME}_image2_all.bin
)
ameba_execute_process(
    COMMAND ${IMAGETOOL} ${c_SDK_IMAGE_TARGET_DIR}/${c_MCU_PROJECT_NAME}_image2_all.bin ${BUILD_TYPE}
    WORKING_DIRECTORY ${c_SOC_PROJECT_DIR}
)

if(CONFIG_WIFI_HOST_CONTROL)
    message("========= linker fullmac image start =========")
    ameba_execute_process(COMMAND ${CMAKE_COMMAND} -E rename ${c_SDK_IMAGE_TARGET_DIR}/fullmac_ram_1_prepend.bin ${c_SDK_IMAGE_TARGET_DIR}/fullmac_img1_all.bin)
    ameba_axf2bin_fw_pack(
        ${c_SDK_IMAGE_TARGET_DIR}/${c_MCU_PROJECT_NAME}_fullmac_img_1.bin
        p_FULLMAC_IMAGE1
            ${c_SDK_IMAGE_TARGET_DIR}/fullmac_img1_all.bin
    )

    if (CONFIG_FULLMAC_NEW_VERSION)
        ameba_execute_process(COMMAND ${CMAKE_COMMAND} -E rename ${c_SDK_IMAGE_TARGET_DIR}/${c_MCU_PROJECT_NAME}_image2_all.bin ${c_SDK_IMAGE_TARGET_DIR}/fullmac_img2_all.bin)
        ameba_axf2bin_fw_pack(
            ${c_SDK_IMAGE_TARGET_DIR}/${c_MCU_PROJECT_NAME}_fullmac_img_2.bin
            p_FULLMAC_IMAGE2
                ${c_SDK_IMAGE_TARGET_DIR}/fullmac_img2_all.bin
        )
    else()
        ameba_execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${c_SDK_IMAGE_TARGET_DIR}/sram_2.bin ${c_SDK_IMAGE_TARGET_DIR}/fullmac_sram_2.bin)
        ameba_axf2bin_pad(${c_SDK_IMAGE_TARGET_DIR}/fullmac_sram_2.bin ${FULLMAC_IMG2_LENGTH})
        ameba_axf2bin_prepend_head(
            ${c_SDK_IMAGE_TARGET_DIR}/fullmac_sram_2_prepend.bin
            ${c_SDK_IMAGE_TARGET_DIR}/fullmac_sram_2.bin
            __sram_image2_start__
            ${c_SDK_IMAGE_TARGET_DIR}/target_img2.map
        )
        ameba_execute_process(COMMAND ${CMAKE_COMMAND} -E rename ${c_SDK_IMAGE_TARGET_DIR}/fullmac_sram_2_prepend.bin ${c_SDK_IMAGE_TARGET_DIR}/fullmac_img2_all.bin)
        ameba_axf2bin_fw_pack(
            ${c_SDK_IMAGE_TARGET_DIR}/${c_MCU_PROJECT_NAME}_fullmac_img_2.bin
            p_FULLMAC_IMAGE2
                ${c_SDK_IMAGE_TARGET_DIR}/fullmac_img2_all.bin
        )
    endif()
    ameba_execute_process(
        COMMAND ${CMAKE_COMMAND} -E cat ${c_SDK_IMAGE_TARGET_DIR}/${c_MCU_PROJECT_NAME}_fullmac_img_2.bin ${c_SDK_IMAGE_TARGET_DIR}/${c_MCU_PROJECT_NAME}_fullmac_img_1.bin
        OUTPUT_FILE ${c_SDK_IMAGE_TARGET_DIR}/${c_MCU_PROJECT_NAME}_fullmac.bin
    )
    ameba_execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${c_SDK_IMAGE_TARGET_DIR}/${c_MCU_PROJECT_NAME}_fullmac.bin ${FINAL_IMAGE_DIR})
    message("========= linker fullmac image end =========")
endif()

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
