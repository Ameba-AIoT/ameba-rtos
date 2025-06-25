#Note: Previously defined variables cannot be used directly in this file
#unless passed through -D
include(${c_CMAKE_FILES_DIR}/axf2bin.cmake)
include(${c_CMAKE_FILES_DIR}/utility.cmake)
include(${c_CMAKE_FILES_DIR}/global_define.cmake)
import_kconfig("CONFIG" ${c_MCU_KCONFIG_FILE})
set(v_USER_CUSTOM_LOG_PREFIX "kr4_POSTBUILD")

execute_process(COMMAND_ERROR_IS_FATAL ANY
    COMMAND ${CMAKE_OBJCOPY} -j .xip_image2.text -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/xip_image2.bin
    COMMAND ${CMAKE_OBJCOPY} -j .psram_image2.text.data -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/psram_2.bin
    COMMAND ${CMAKE_OBJCOPY} -j .sram_image2.text.data -j .ram_image2_fill -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/sram_2.bin
    COMMAND ${CMAKE_OBJCOPY} -j .boot.entry -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/boot.bin
    COMMAND ${CMAKE_OBJCOPY} -j .sram_only.text.data -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/sram_only.bin
    COMMAND ${CMAKE_OBJCOPY} -j .ram_retention.text -j .ram_retention.entry -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/ram_retention.bin
)

if(CONFIG_BT)
    execute_process(COMMAND_ERROR_IS_FATAL ANY COMMAND ${CMAKE_OBJCOPY} -j .bluetooth_trace.text -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/APP.trace)
endif()

if(CONFIG_BT_COEXIST AND CONFIG_COEXIST_DEV)
    execute_process(COMMAND_ERROR_IS_FATAL ANY
        COMMAND ${CMAKE_OBJCOPY} -j .coex_trace.text -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/COEX.trace
    )
endif()

message( "========== Image manipulating start ==========")

if (CONFIG_DSP_WITHIN_APP_IMG)
    if(NOT EXISTS ${c_SOC_PROJECT_DIR}/${CONFIG_DSP_IMAGE_TARGET_DIR}/dsp.bin)
        message(FATAL_ERROR "no dsp.bin in ${CONFIG_DSP_IMAGE_TARGET_DIR}")
    endif()
endif()

execute_process(COMMAND_ERROR_IS_FATAL ANY
    COMMAND ${PADTOOL} ${c_SDK_IMAGE_TARGET_DIR}/xip_image2.bin  32
    COMMAND ${PADTOOL} ${c_SDK_IMAGE_TARGET_DIR}/psram_2.bin  32
    COMMAND ${PADTOOL} ${c_SDK_IMAGE_TARGET_DIR}/sram_2.bin 32
    COMMAND ${PADTOOL} ${c_SDK_IMAGE_TARGET_DIR}/boot.bin  32
    COMMAND ${PADTOOL} ${c_SDK_IMAGE_TARGET_DIR}/sram_only.bin  32
)

if(CONFIG_MP_SHRINK)
    if(CONFIG_AS_INIC_AP)
        message(FATAL_ERROR "Setting KR4 as AP is NOT ALLOWED in SHRINK MP.")
    endif()
endif()

execute_process(COMMAND_ERROR_IS_FATAL ANY
    COMMAND ${PREPENDTOOL} ${c_SDK_IMAGE_TARGET_DIR}/xip_image2.bin  __flash_text_start__  ${c_SDK_IMAGE_TARGET_DIR}/target_img2.map
    COMMAND ${PREPENDTOOL} ${c_SDK_IMAGE_TARGET_DIR}/psram_2.bin  __psram_image2_start__  ${c_SDK_IMAGE_TARGET_DIR}/target_img2.map
    COMMAND ${PREPENDTOOL} ${c_SDK_IMAGE_TARGET_DIR}/sram_2.bin  __sram_image2_start__  ${c_SDK_IMAGE_TARGET_DIR}/target_img2.map
    COMMAND ${PREPENDTOOL} ${c_SDK_IMAGE_TARGET_DIR}/boot.bin  __boot_text_start__  ${c_SDK_IMAGE_TARGET_DIR}/target_img2.map
    COMMAND ${PREPENDTOOL} ${c_SDK_IMAGE_TARGET_DIR}/sram_only.bin __sram_only_start__  ${c_SDK_IMAGE_TARGET_DIR}/target_img2.map
)
# pad to 4K-aligned for RSIP/MMU address 4K-alignment restriction
execute_process(COMMAND_ERROR_IS_FATAL ANY
    COMMAND ${CMAKE_COMMAND} -E cat ${c_SDK_IMAGE_TARGET_DIR}/xip_image2_prepend.bin ${c_SDK_IMAGE_TARGET_DIR}/psram_2_prepend.bin ${c_SDK_IMAGE_TARGET_DIR}/sram_2_prepend.bin ${c_SDK_IMAGE_TARGET_DIR}/boot_prepend.bin ${c_SDK_IMAGE_TARGET_DIR}/sram_only_prepend.bin
    OUTPUT_FILE ${c_SDK_IMAGE_TARGET_DIR}/kr4_image2_all.bin
)
execute_process(COMMAND_ERROR_IS_FATAL ANY COMMAND ${PADTOOL} ${c_SDK_IMAGE_TARGET_DIR}/kr4_image2_all.bin 4096)
execute_process(COMMAND_ERROR_IS_FATAL ANY
    COMMAND ${IMAGETOOL} ${c_SDK_IMAGE_TARGET_DIR}/kr4_image2_all.bin ${BUILD_TYPE} ${CONFIG_DSP_IMAGE_TARGET_DIR}
    WORKING_DIRECTORY ${c_SOC_PROJECT_DIR}
)

if(CONFIG_FATFS_WITHIN_APP_IMG)
    if(EXISTS ${KM4_PROJECT_DIR}/asdk/image/kr4_km4_app.bin AND EXISTS ${c_BASEDIR}/amebalite_gcc_project/fatfs.bin)
        execute_process(COMMAND_ERROR_IS_FATAL ANY
            COMMAND ${CMAKE_COMMAND} -E rename ${KM4_PROJECT_DIR}/asdk/image/kr4_km4_app.bin ${KM4_PROJECT_DIR}/asdk/image/kr4_km4_app_tmp.bin
            COMMAND ${CMAKE_COMMAND} -E rename ${KM4_PROJECT_DIR}/asdk/image/kr4_km4_app_ns.bin ${KM4_PROJECT_DIR}/asdk/image/kr4_km4_app_ns_tmp.bin
        )
        execute_process(COMMAND_ERROR_IS_FATAL ANY
            COMMAND ${PADTOOL} ${KM4_PROJECT_DIR}/asdk/image/kr4_km4_app_tmp.bin 4096
            COMMAND ${PADTOOL} ${KM4_PROJECT_DIR}/asdk/image/kr4_km4_app_ns_tmp.bin 4096
            COMMAND ${PREPENDTOOL} ${c_BASEDIR}/amebalite_gcc_project/fatfs.bin  VFS1_FLASH_BASE_ADDR  ${AP_IMAGEDIR}/target_img2.map
        )
        execute_process(COMMAND_ERROR_IS_FATAL ANY
            COMMAND ${CMAKE_COMMAND} -E cat ${KM4_PROJECT_DIR}/asdk/image/kr4_km4_app_tmp.bin ${c_BASEDIR}/amebalite_gcc_project/fatfs_prepend.bin
            OUTPUT_FILE ${KM4_PROJECT_DIR}/asdk/image/kr4_km4_app.bin
        )
        execute_process(COMMAND_ERROR_IS_FATAL ANY
            COMMAND ${CMAKE_COMMAND} -E cat ${KM4_PROJECT_DIR}/asdk/image/kr4_km4_app_ns_tmp.bin ${c_BASEDIR}/amebalite_gcc_project/fatfs_prepend.bin
            OUTPUT_FILE ${KM4_PROJECT_DIR}/asdk/image/kr4_km4_app_ns.bin
        )
    else()
        if(EXISTS ${KM4_PROJECT_DIR}/asdk/image/kr4_km4_dsp_app.bin AND EXISTS ${c_BASEDIR}/amebalite_gcc_project/fatfs.bin)
            execute_process(COMMAND_ERROR_IS_FATAL ANY
                COMMAND ${CMAKE_COMMAND} -E rename ${KM4_PROJECT_DIR}/asdk/image/kr4_km4_dsp_app.bin ${KM4_PROJECT_DIR}/asdk/image/kr4_km4_dsp_app_tmp.bin
                COMMAND ${CMAKE_COMMAND} -E rename ${KM4_PROJECT_DIR}/asdk/image/kr4_km4_dsp_app_ns.bin ${KM4_PROJECT_DIR}/asdk/image/kr4_km4_dsp_app_ns_tmp.bin
            )
            execute_process(COMMAND_ERROR_IS_FATAL ANY
                COMMAND ${PADTOOL} ${KM4_PROJECT_DIR}/asdk/image/kr4_km4_dsp_app_tmp.bin 4096
                COMMAND ${PADTOOL} ${KM4_PROJECT_DIR}/asdk/image/kr4_km4_dsp_app_ns_tmp.bin 4096
                COMMAND ${PREPENDTOOL} ${c_BASEDIR}/amebalite_gcc_project/fatfs.bin  VFS1_FLASH_BASE_ADDR  ${AP_IMAGEDIR}/target_img2.map
            )
            execute_process(COMMAND_ERROR_IS_FATAL ANY
                COMMAND ${CMAKE_COMMAND} -E cat ${KM4_PROJECT_DIR}/asdk/image/kr4_km4_dsp_app_tmp.bin ${c_BASEDIR}/amebalite_gcc_project/fatfs_prepend.bin
                OUTPUT_FILE ${KM4_PROJECT_DIR}/asdk/image/kr4_km4_dsp_app.bin
            )
            execute_process(COMMAND_ERROR_IS_FATAL ANY
                COMMAND ${CMAKE_COMMAND} -E cat ${KM4_PROJECT_DIR}/asdk/image/kr4_km4_dsp_app_ns_tmp.bin ${c_BASEDIR}/amebalite_gcc_project/fatfs_prepend.bin
                OUTPUT_FILE ${KM4_PROJECT_DIR}/asdk/image/kr4_km4_dsp_app_ns.bin
            )
        endif()
    endif()
endif()

if(CONFIG_COMPRESS_OTA_IMG)
    if(EXISTS ${KM4_PROJECT_DIR}/asdk/image/kr4_km4_app.bin)
        execute_process(COMMAND_ERROR_IS_FATAL ANY
            COMMAND ${COMPRESSTOOL} ${KM4_PROJECT_DIR}/asdk/image/kr4_km4_app.bin
            COMMAND ${COMPRESSTOOL} ${KM4_PROJECT_DIR}/asdk/image/kr4_km4_app_ns.bin
        )
        execute_process(COMMAND_ERROR_IS_FATAL ANY
            COMMAND ${CMAKE_COMMAND} -E copy ${KM4_PROJECT_DIR}/asdk/image/kr4_km4_app_compress.bin ${KM4_PROJECT_DIR}/asdk/image/tmp_app.bin
            COMMAND ${CMAKE_COMMAND} -E copy ${KM4_PROJECT_DIR}/asdk/image/kr4_km4_app_ns_compress.bin ${KM4_PROJECT_DIR}/asdk/image/tmp_app_ns.bin
        )
    else()
        if(EXISTS ${KM4_PROJECT_DIR}/asdk/image/kr4_km4_dsp_app.bin)
            execute_process(COMMAND_ERROR_IS_FATAL ANY
                COMMAND ${COMPRESSTOOL} ${KM4_PROJECT_DIR}/asdk/image/kr4_km4_dsp_app.bin
                COMMAND ${COMPRESSTOOL} ${KM4_PROJECT_DIR}/asdk/image/kr4_km4_dsp_app_ns.bin
            )
            execute_process(COMMAND_ERROR_IS_FATAL ANY
                COMMAND ${CMAKE_COMMAND} -E copy ${KM4_PROJECT_DIR}/asdk/image/kr4_km4_dsp_app_compress.bin ${KM4_PROJECT_DIR}/asdk/image/tmp_app.bin
                COMMAND ${CMAKE_COMMAND} -E copy ${KM4_PROJECT_DIR}/asdk/image/kr4_km4_dsp_app_ns_compress.bin ${KM4_PROJECT_DIR}/asdk/image/tmp_app_ns.bin
            )
        endif()
    endif()
else()
    if(EXISTS ${KM4_PROJECT_DIR}/asdk/image/kr4_km4_app.bin)
        execute_process(COMMAND_ERROR_IS_FATAL ANY
            COMMAND ${CMAKE_COMMAND} -E copy ${KM4_PROJECT_DIR}/asdk/image/kr4_km4_app.bin ${KM4_PROJECT_DIR}/asdk/image/tmp_app.bin
            COMMAND ${CMAKE_COMMAND} -E copy ${KM4_PROJECT_DIR}/asdk/image/kr4_km4_app_ns.bin ${KM4_PROJECT_DIR}/asdk/image/tmp_app_ns.bin
        )
    else()
        if(EXISTS ${KM4_PROJECT_DIR}/asdk/image/kr4_km4_dsp_app.bin)
            execute_process(COMMAND_ERROR_IS_FATAL ANY
                COMMAND ${CMAKE_COMMAND} -E copy ${KM4_PROJECT_DIR}/asdk/image/kr4_km4_dsp_app.bin ${KM4_PROJECT_DIR}/asdk/image/tmp_app.bin
                COMMAND ${CMAKE_COMMAND} -E copy ${KM4_PROJECT_DIR}/asdk/image/kr4_km4_dsp_app_ns.bin ${KM4_PROJECT_DIR}/asdk/image/tmp_app_ns.bin
            )
        endif()
    endif()
endif()

if(CONFIG_UPGRADE_BOOTLOADER)
    if(EXISTS ${KM4_PROJECT_DIR}/asdk/image/tmp_app.bin)
        execute_process(COMMAND_ERROR_IS_FATAL ANY
            COMMAND ${OTAPREPENDTOOL} ${KM4_PROJECT_DIR}/asdk/image/tmp_app.bin ${KM4_PROJECT_DIR}/asdk/image/km4_boot_all.bin
            COMMAND ${OTAPREPENDTOOL} ${KM4_PROJECT_DIR}/asdk/image/tmp_app_ns.bin ${KM4_PROJECT_DIR}/asdk/image/km4_boot_all_ns.bin
        )
    endif()
else()
    if(EXISTS ${KM4_PROJECT_DIR}/asdk/image/tmp_app.bin)
        execute_process(COMMAND_ERROR_IS_FATAL ANY
            COMMAND ${OTAPREPENDTOOL} ${KM4_PROJECT_DIR}/asdk/image/tmp_app.bin
            COMMAND ${OTAPREPENDTOOL} ${KM4_PROJECT_DIR}/asdk/image/tmp_app_ns.bin
        )
    endif()
endif()

execute_process(COMMAND_ERROR_IS_FATAL ANY
    COMMAND ${CMAKE_COMMAND} -E remove ${KM4_PROJECT_DIR}/asdk/image/tmp_app.bin
    COMMAND ${CMAKE_COMMAND} -E remove ${KM4_PROJECT_DIR}/asdk/image/tmp_app_ns.bin
)
message("========== Image manipulating end ==========")

if (CONFIG_MP_INCLUDED)
    set(APP_ALL ${KM4_PROJECT_DIR}/asdk/image_mp/kr4_km4_app_mp.bin)
elseif(CONFIG_DSP_WITHIN_APP_IMG)
    set(APP_ALL ${KM4_PROJECT_DIR}/asdk/image/kr4_km4_dsp_app.bin)
else()
    set(APP_ALL	${KM4_PROJECT_DIR}/asdk/image/kr4_km4_app.bin)
endif()
set(OTA_ALL	${KM4_PROJECT_DIR}/asdk/image/ota_all.bin)

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
        WORKING_DIRECTORY ${c_MCU_PROJECT_DIR}/vsdk
    )
    execute_process(COMMAND_ERROR_IS_FATAL ANY
        COMMAND ${STATIC_ANALYZE_PYTHON} ${DAILY_BUILD}
        WORKING_DIRECTORY ${c_MCU_PROJECT_DIR}/vsdk
    )
    message("========== Image analyze end ==========")
endif()
