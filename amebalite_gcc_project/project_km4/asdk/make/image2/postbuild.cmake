#Note: Previously defined variables cannot be used directly in this file
#unless passed through -D
cmake_minimum_required(VERSION 3.20.0)
include(${c_CMAKE_FILES_DIR}/axf2bin.cmake)
include(${c_CMAKE_FILES_DIR}/utility.cmake)
include(${c_CMAKE_FILES_DIR}/global_define.cmake)
import_kconfig("CONFIG" ${c_MCU_KCONFIG_FILE})
set(v_USER_CUSTOM_LOG_PREFIX "km4_POSTBUILD")

execute_process(COMMAND_ERROR_IS_FATAL ANY
    COMMAND ${CMAKE_OBJCOPY} -j .boot.entry -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/boot.bin
    COMMAND ${CMAKE_OBJCOPY} -j .sram_only.text.data -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/sram_only.bin
    COMMAND ${CMAKE_OBJCOPY} -j .sram_image2.text.data  -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/sram_2.bin
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
    execute_process(COMMAND_ERROR_IS_FATAL ANY
        COMMAND ${CMAKE_OBJCOPY} -j .psram_image2.text.data -j .ARM.extab -j .ARM.exidx -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/psram_2.bin
        COMMAND ${CMAKE_OBJCOPY} -j .xip_image2.text -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/xip_image2.bin
    )
elseif(0x${ARMExAddr} LESS 0x20000000)
    execute_process(COMMAND_ERROR_IS_FATAL ANY
        COMMAND ${CMAKE_OBJCOPY} -j .psram_image2.text.data -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/psram_2.bin
        COMMAND ${CMAKE_OBJCOPY} -j .xip_image2.text -j .ARM.extab -j .ARM.exidx -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/xip_image2.bin
    )
else()
    execute_process(COMMAND_ERROR_IS_FATAL ANY
        COMMAND ${CMAKE_OBJCOPY} -j .psram_image2.text.data -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/psram_2.bin
        COMMAND ${CMAKE_OBJCOPY} -j .xip_image2.text -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/xip_image2.bin
    )
endif()

if(CONFIG_BT)
    execute_process(COMMAND_ERROR_IS_FATAL ANY
        COMMAND ${CMAKE_OBJCOPY} -j .bluetooth_trace.text -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/APP.trace
    )
endif()

if(CONFIG_BT_COEXIST AND CONFIG_COEXIST_DEV)
    execute_process(COMMAND_ERROR_IS_FATAL ANY
        COMMAND ${CMAKE_OBJCOPY} -j .coex_trace.text -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/COEX.trace
    )
endif()

message( "========== Image manipulating start ==========")

if (CONFIG_DSP_WITHIN_APP_IMG)
    if(NOT EXISTS ${c_MCU_PROJECT_DIR}/../${CONFIG_DSP_IMAGE_TARGET_DIR}/dsp.bin)
        message(FATAL_ERROR "no dsp.bin in ${CONFIG_DSP_IMAGE_TARGET_DIR}")
    endif()
endif()

execute_process(COMMAND_ERROR_IS_FATAL ANY
    COMMAND ${PADTOOL} ${c_SDK_IMAGE_TARGET_DIR}/boot.bin  32
    COMMAND ${PADTOOL} ${c_SDK_IMAGE_TARGET_DIR}/sram_only.bin  32
    COMMAND ${PADTOOL} ${c_SDK_IMAGE_TARGET_DIR}/sram_2.bin 32
    COMMAND ${PADTOOL} ${c_SDK_IMAGE_TARGET_DIR}/psram_2.bin  32
    COMMAND ${PADTOOL} ${c_SDK_IMAGE_TARGET_DIR}/xip_image2.bin  32
)

execute_process(COMMAND_ERROR_IS_FATAL ANY
    COMMAND ${PREPENDTOOL} ${c_SDK_IMAGE_TARGET_DIR}/boot.bin  __boot_text_start__  ${c_SDK_IMAGE_TARGET_DIR}/target_img2.map
    COMMAND ${PREPENDTOOL} ${c_SDK_IMAGE_TARGET_DIR}/sram_only.bin __sram_only_start__  ${c_SDK_IMAGE_TARGET_DIR}/target_img2.map
    COMMAND ${PREPENDTOOL} ${c_SDK_IMAGE_TARGET_DIR}/sram_2.bin  __sram_image2_start__  ${c_SDK_IMAGE_TARGET_DIR}/target_img2.map
    COMMAND ${PREPENDTOOL} ${c_SDK_IMAGE_TARGET_DIR}/psram_2.bin  __psram_image2_start__  ${c_SDK_IMAGE_TARGET_DIR}/target_img2.map
    COMMAND ${PREPENDTOOL} ${c_SDK_IMAGE_TARGET_DIR}/xip_image2.bin  __flash_text_start__  ${c_SDK_IMAGE_TARGET_DIR}/target_img2.map
)

execute_process(COMMAND_ERROR_IS_FATAL ANY
    COMMAND ${CMAKE_COMMAND} -E cat ${c_SDK_IMAGE_TARGET_DIR}/xip_image2_prepend.bin ${c_SDK_IMAGE_TARGET_DIR}/psram_2_prepend.bin ${c_SDK_IMAGE_TARGET_DIR}/sram_2_prepend.bin ${c_SDK_IMAGE_TARGET_DIR}/boot_prepend.bin ${c_SDK_IMAGE_TARGET_DIR}/sram_only_prepend.bin
    OUTPUT_FILE ${c_SDK_IMAGE_TARGET_DIR}/km4_image2_all.bin
)

execute_process(COMMAND_ERROR_IS_FATAL ANY
    COMMAND ${IMAGETOOL} ${c_SDK_IMAGE_TARGET_DIR}/km4_image2_all.bin ${BUILD_TYPE} ${CONFIG_DSP_IMAGE_TARGET_DIR}
    WORKING_DIRECTORY ${c_MCU_PROJECT_DIR}/..
)

if(CONFIG_FATFS_WITHIN_APP_IMG)
    if(EXISTS ${c_SDK_IMAGE_TARGET_DIR}/kr4_km4_app.bin AND EXISTS ${c_BASEDIR}/amebalite_gcc_project/fatfs.bin)
        execute_process(COMMAND_ERROR_IS_FATAL ANY
            COMMAND ${CMAKE_COMMAND} -E rename ${c_SDK_IMAGE_TARGET_DIR}/kr4_km4_app.bin ${c_SDK_IMAGE_TARGET_DIR}/kr4_km4_app_tmp.bin
            COMMAND ${CMAKE_COMMAND} -E rename ${c_SDK_IMAGE_TARGET_DIR}/kr4_km4_app_ns.bin ${c_SDK_IMAGE_TARGET_DIR}/kr4_km4_app_ns_tmp.bin
        )
        execute_process(COMMAND_ERROR_IS_FATAL ANY
            COMMAND ${PADTOOL} ${c_SDK_IMAGE_TARGET_DIR}/kr4_km4_app_tmp.bin 4096
            COMMAND ${PADTOOL} ${c_SDK_IMAGE_TARGET_DIR}/kr4_km4_app_ns_tmp.bin 4096
            COMMAND ${PREPENDTOOL} ${c_BASEDIR}/amebalite_gcc_project/fatfs.bin  VFS1_FLASH_BASE_ADDR  ${AP_IMAGEDIR}/target_img2.map
        )
        execute_process(COMMAND_ERROR_IS_FATAL ANY
            COMMAND ${CMAKE_COMMAND} -E cat ${c_SDK_IMAGE_TARGET_DIR}/kr4_km4_app_tmp.bin ${c_BASEDIR}/amebalite_gcc_project/fatfs_prepend.bin
            OUTPUT_FILE ${c_SDK_IMAGE_TARGET_DIR}/kr4_km4_app.bin
        )
        execute_process(COMMAND_ERROR_IS_FATAL ANY
            COMMAND ${CMAKE_COMMAND} -E cat ${c_SDK_IMAGE_TARGET_DIR}/kr4_km4_app_ns_tmp.bin ${c_BASEDIR}/amebalite_gcc_project/fatfs_prepend.bin
            OUTPUT_FILE ${c_SDK_IMAGE_TARGET_DIR}/kr4_km4_app_ns.bin
        )
    else()
        if(EXISTS ${c_SDK_IMAGE_TARGET_DIR}/kr4_km4_dsp_app.bin AND EXISTS ${c_BASEDIR}/amebalite_gcc_project/fatfs.bin)
            execute_process(COMMAND_ERROR_IS_FATAL ANY
                COMMAND ${CMAKE_COMMAND} -E rename ${c_SDK_IMAGE_TARGET_DIR}/kr4_km4_dsp_app.bin ${c_SDK_IMAGE_TARGET_DIR}/kr4_km4_dsp_app_tmp.bin
                COMMAND ${CMAKE_COMMAND} -E rename ${c_SDK_IMAGE_TARGET_DIR}/kr4_km4_dsp_app_ns.bin ${c_SDK_IMAGE_TARGET_DIR}/kr4_km4_dsp_app_ns_tmp.bin
            )
            execute_process(COMMAND_ERROR_IS_FATAL ANY
                COMMAND ${PADTOOL} ${c_SDK_IMAGE_TARGET_DIR}/kr4_km4_dsp_app_tmp.bin 4096
                COMMAND ${PADTOOL} ${c_SDK_IMAGE_TARGET_DIR}/kr4_km4_dsp_app_ns_tmp.bin 4096
                COMMAND ${PREPENDTOOL} ${c_BASEDIR}/amebalite_gcc_project/fatfs.bin  VFS1_FLASH_BASE_ADDR  ${AP_IMAGEDIR}/target_img2.map
            )
            execute_process(COMMAND_ERROR_IS_FATAL ANY
                COMMAND ${CMAKE_COMMAND} -E cat ${c_SDK_IMAGE_TARGET_DIR}/kr4_km4_dsp_app_tmp.bin ${c_BASEDIR}/amebalite_gcc_project/fatfs_prepend.bin
                OUTPUT_FILE ${c_SDK_IMAGE_TARGET_DIR}/kr4_km4_dsp_app.bin
            )
            execute_process(COMMAND_ERROR_IS_FATAL ANY
                COMMAND ${CMAKE_COMMAND} -E cat ${c_SDK_IMAGE_TARGET_DIR}/kr4_km4_dsp_app_ns_tmp.bin ${c_BASEDIR}/amebalite_gcc_project/fatfs_prepend.bin
                OUTPUT_FILE ${c_SDK_IMAGE_TARGET_DIR}/kr4_km4_dsp_app_ns.bin
            )
        else()
            message(FATAL_ERROR "Lack of fatfs.bin when enabling CONFIG_FATFS_WITHIN_APP_IMG")
        endif()
    endif()
endif()

if(CONFIG_COMPRESS_OTA_IMG)
    if(EXISTS ${c_SDK_IMAGE_TARGET_DIR}/kr4_km4_app.bin)
        execute_process(COMMAND_ERROR_IS_FATAL ANY
            COMMAND ${COMPRESSTOOL} ${c_SDK_IMAGE_TARGET_DIR}/kr4_km4_app.bin
            COMMAND ${COMPRESSTOOL} ${c_SDK_IMAGE_TARGET_DIR}/kr4_km4_app_ns.bin
        )
        execute_process(COMMAND_ERROR_IS_FATAL ANY
            COMMAND ${CMAKE_COMMAND} -E copy ${c_SDK_IMAGE_TARGET_DIR}/kr4_km4_app_compress.bin ${c_SDK_IMAGE_TARGET_DIR}/tmp_app.bin
            COMMAND ${CMAKE_COMMAND} -E copy ${c_SDK_IMAGE_TARGET_DIR}/kr4_km4_app_ns_compress.bin ${c_SDK_IMAGE_TARGET_DIR}/tmp_app_ns.bin
        )
    else()
        if(EXISTS ${c_SDK_IMAGE_TARGET_DIR}/kr4_km4_dsp_app.bin)
            execute_process(COMMAND_ERROR_IS_FATAL ANY
                COMMAND ${COMPRESSTOOL} ${c_SDK_IMAGE_TARGET_DIR}/kr4_km4_dsp_app.bin
                COMMAND ${COMPRESSTOOL} ${c_SDK_IMAGE_TARGET_DIR}/kr4_km4_dsp_app_ns.bin
            )
            execute_process(COMMAND_ERROR_IS_FATAL ANY
                COMMAND ${CMAKE_COMMAND} -E copy ${c_SDK_IMAGE_TARGET_DIR}/kr4_km4_dsp_app_compress.bin ${c_SDK_IMAGE_TARGET_DIR}/tmp_app.bin
                COMMAND ${CMAKE_COMMAND} -E copy ${c_SDK_IMAGE_TARGET_DIR}/kr4_km4_dsp_app_ns_compress.bin ${c_SDK_IMAGE_TARGET_DIR}/tmp_app_ns.bin
            )
        endif()
    endif()
else()
    if(EXISTS ${c_SDK_IMAGE_TARGET_DIR}/kr4_km4_app.bin)
        execute_process(COMMAND_ERROR_IS_FATAL ANY
            COMMAND ${CMAKE_COMMAND} -E copy ${c_SDK_IMAGE_TARGET_DIR}/kr4_km4_app.bin ${c_SDK_IMAGE_TARGET_DIR}/tmp_app.bin
            COMMAND ${CMAKE_COMMAND} -E copy ${c_SDK_IMAGE_TARGET_DIR}/kr4_km4_app_ns.bin ${c_SDK_IMAGE_TARGET_DIR}/tmp_app_ns.bin
        )
    else()
        if(EXISTS ${c_SDK_IMAGE_TARGET_DIR}/kr4_km4_dsp_app.bin)
            execute_process(COMMAND_ERROR_IS_FATAL ANY
                COMMAND ${CMAKE_COMMAND} -E copy ${c_SDK_IMAGE_TARGET_DIR}/kr4_km4_dsp_app.bin ${c_SDK_IMAGE_TARGET_DIR}/tmp_app.bin
                COMMAND ${CMAKE_COMMAND} -E copy ${c_SDK_IMAGE_TARGET_DIR}/kr4_km4_dsp_app_ns.bin ${c_SDK_IMAGE_TARGET_DIR}/tmp_app_ns.bin
            )
        endif()
    endif()
endif()

if(CONFIG_UPGRADE_BOOTLOADER)
    if(EXISTS ${c_SDK_IMAGE_TARGET_DIR}/tmp_app.bin)
        execute_process(COMMAND_ERROR_IS_FATAL ANY
            COMMAND ${OTAPREPENDTOOL} ${c_SDK_IMAGE_TARGET_DIR}/tmp_app.bin ${c_SDK_IMAGE_TARGET_DIR}/km4_boot_all.bin
            COMMAND ${OTAPREPENDTOOL} ${c_SDK_IMAGE_TARGET_DIR}/tmp_app_ns.bin ${c_SDK_IMAGE_TARGET_DIR}/km4_boot_all_ns.bin
        )
    endif()
else()
    if(EXISTS ${c_SDK_IMAGE_TARGET_DIR}/tmp_app.bin)
        #FIXME: This step may cause io error, Add "COMMAND_ERROR_IS_FATAL ANY" for execute_process to see
        execute_process(
            COMMAND ${OTAPREPENDTOOL} ${c_SDK_IMAGE_TARGET_DIR}/tmp_app.bin
            COMMAND ${OTAPREPENDTOOL} ${c_SDK_IMAGE_TARGET_DIR}/tmp_app_ns.bin
        )
    endif()
endif()

execute_process(COMMAND_ERROR_IS_FATAL ANY
    COMMAND ${CMAKE_COMMAND} -E remove ${c_SDK_IMAGE_TARGET_DIR}/tmp_app.bin
    COMMAND ${CMAKE_COMMAND} -E remove ${c_SDK_IMAGE_TARGET_DIR}/tmp_app_ns.bin
)
message("========== Image manipulating end ==========")

if (CONFIG_MP_INCLUDED)
    set(APP_ALL ${c_SDK_IMAGE_TARGET_DIR}/kr4_km4_app_mp.bin)
elseif(CONFIG_DSP_WITHIN_APP_IMG)
    set(APP_ALL ${c_SDK_IMAGE_TARGET_DIR}/kr4_km4_dsp_app.bin)
else()
    set(APP_ALL	${c_SDK_IMAGE_TARGET_DIR}/kr4_km4_app.bin)
endif()
set(OTA_ALL	${c_SDK_IMAGE_TARGET_DIR}/ota_all.bin)

if(EXISTS ${APP_ALL})
    execute_process(COMMAND_ERROR_IS_FATAL ANY COMMAND ${CMAKE_COMMAND} -E copy ${APP_ALL} ${FINAL_IMAGE_DIR})
endif()

if(EXISTS ${OTA_ALL})
    execute_process(COMMAND_ERROR_IS_FATAL ANY COMMAND ${CMAKE_COMMAND} -E copy ${OTA_ALL} ${FINAL_IMAGE_DIR})
endif()

if(NOT CONFIG_AMEBA_RLS)
    message("========== Image analyze start ==========")
    execute_process(COMMAND_ERROR_IS_FATAL ANY
        COMMAND ${CODE_ANALYZE_PYTHON} ${ANALYZE_MP_IMG} ${DAILY_BUILD}
        WORKING_DIRECTORY ${c_MCU_PROJECT_DIR}/asdk
    )
    execute_process(COMMAND_ERROR_IS_FATAL ANY
        COMMAND ${STATIC_ANALYZE_PYTHON} ${DAILY_BUILD}
        WORKING_DIRECTORY ${c_MCU_PROJECT_DIR}/asdk
    )
    message("========== Image analyze end ==========")
endif()
