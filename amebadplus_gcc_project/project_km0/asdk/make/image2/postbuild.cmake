#Note: Previously defined variables cannot be used directly in this file
#unless passed through -D
include(${c_CMAKE_FILES_DIR}/axf2bin.cmake)
include(${c_CMAKE_FILES_DIR}/global_define.cmake)
include(${c_CMAKE_FILES_DIR}/utility.cmake)
import_kconfig("CONFIG" ${c_MCU_KCONFIG_FILE})

execute_process(
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
    execute_process(
        COMMAND ${CMAKE_OBJCOPY} -j .psram_image2.text.data -j .ARM.extab -j .ARM.exidx -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/psram_2.bin
        COMMAND ${CMAKE_OBJCOPY} -j .xip_image2.text -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/xip_image2.bin
    )
elseif(0x${ARMExAddr} LESS 0x20000000)
    execute_process(
        COMMAND ${CMAKE_OBJCOPY} -j .psram_image2.text.data -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/psram_2.bin
        COMMAND ${CMAKE_OBJCOPY} -j .xip_image2.text -j .ARM.extab -j .ARM.exidx -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/xip_image2.bin
    )
else()
    execute_process(
        COMMAND ${CMAKE_OBJCOPY} -j .psram_image2.text.data -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/psram_2.bin
        COMMAND ${CMAKE_OBJCOPY} -j .xip_image2.text -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/xip_image2.bin
    )
endif()

execute_process(
    COMMAND ${CMAKE_OBJCOPY} -j .ram_retention.text -j .ram_retention.entry -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/ram_retention.bin
)

if(CONFIG_BT_COEXIST AND CONFIG_COEXIST_DEV)
execute_process(
    COMMAND ${CMAKE_OBJCOPY} -j .coex_trace.text -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/COEX.trace
)
endif()

message( "========== Image manipulating start ==========")

execute_process(
    COMMAND ${PADTOOL} ${c_SDK_IMAGE_TARGET_DIR}/xip_image2.bin 32
    COMMAND ${PADTOOL} ${c_SDK_IMAGE_TARGET_DIR}/sram_2.bin 32
	COMMAND ${PADTOOL} ${c_SDK_IMAGE_TARGET_DIR}/psram_2.bin 32
	COMMAND ${PADTOOL} ${c_SDK_IMAGE_TARGET_DIR}/entry.bin 32
)

execute_process(
	COMMAND ${PREPENDTOOL} ${c_SDK_IMAGE_TARGET_DIR}/xip_image2.bin  __flash_text_start__  ${c_SDK_IMAGE_TARGET_DIR}/target_img2.map
	COMMAND ${PREPENDTOOL} ${c_SDK_IMAGE_TARGET_DIR}/sram_2.bin  __sram_image2_start__  ${c_SDK_IMAGE_TARGET_DIR}/target_img2.map
	COMMAND ${PREPENDTOOL} ${c_SDK_IMAGE_TARGET_DIR}/psram_2.bin  __psram_image2_start__  ${c_SDK_IMAGE_TARGET_DIR}/target_img2.map
	COMMAND ${PREPENDTOOL} ${c_SDK_IMAGE_TARGET_DIR}/entry.bin  __img2_entry_start__  ${c_SDK_IMAGE_TARGET_DIR}/target_img2.map
)

execute_process(
    COMMAND ${CMAKE_COMMAND} -E cat ${c_SDK_IMAGE_TARGET_DIR}/xip_image2_prepend.bin ${c_SDK_IMAGE_TARGET_DIR}/sram_2_prepend.bin ${c_SDK_IMAGE_TARGET_DIR}/psram_2_prepend.bin ${c_SDK_IMAGE_TARGET_DIR}/entry_prepend.bin
    OUTPUT_FILE ${c_SDK_IMAGE_TARGET_DIR}/km0_image2_all.bin
)
execute_process(
    COMMAND ${PADTOOL} ${c_SDK_IMAGE_TARGET_DIR}/km0_image2_all.bin 4096
)
execute_process(
    COMMAND ${IMAGETOOL} ${c_SDK_IMAGE_TARGET_DIR}/km0_image2_all.bin ${BUILD_TYPE}
    WORKING_DIRECTORY ${c_MCU_PROJECT_DIR}/..
)

if(CONFIG_FATFS_WITHIN_APP_IMG)
    if(EXISTS ${KM4_PROJECT_DIR}/asdk/image/km0_km4_app.bin AND EXISTS ${c_BASEDIR}/amebadplus_gcc_project/fatfs.bin)
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E rename ${KM4_PROJECT_DIR}/asdk/image/km0_km4_app.bin ${KM4_PROJECT_DIR}/asdk/image/km0_km4_app_tmp.bin
        )
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E rename ${KM4_PROJECT_DIR}/asdk/image/km0_km4_app_ns.bin ${KM4_PROJECT_DIR}/asdk/image/km0_km4_app_ns_tmp.bin
        )
        execute_process(
            COMMAND ${PADTOOL} ${KM4_PROJECT_DIR}/asdk/image/km0_km4_app_tmp.bin 4096
        )
        execute_process(
            COMMAND ${PADTOOL} ${KM4_PROJECT_DIR}/asdk/image/km0_km4_app_ns_tmp.bin 4096
        )
        execute_process(
            COMMAND ${PREPENDTOOL} ${c_BASEDIR}/amebadplus_gcc_project/fatfs.bin  VFS1_FLASH_BASE_ADDR  ${KM4_PROJECT_DIR}/asdk/image/target_img2.map
        )
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E cat ${KM4_PROJECT_DIR}/asdk/image/km0_km4_app_tmp.bin ${c_BASEDIR}/amebadplus_gcc_project/fatfs_prepend.bin
            OUTPUT_FILE ${KM4_PROJECT_DIR}/asdk/image/km0_km4_app.bin
        )
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E cat ${KM4_PROJECT_DIR}/asdk/image/km0_km4_app_ns_tmp.bin ${c_BASEDIR}/amebadplus_gcc_project/fatfs_prepend.bin
            OUTPUT_FILE ${KM4_PROJECT_DIR}/asdk/image/km0_km4_app_ns.bin
        )
    else()
        message(FATAL_ERROR "Lack of fatfs.bin when enabling CONFIG_FATFS_WITHIN_APP_IMG")
    endif()
endif()

if(CONFIG_COMPRESS_OTA_IMG)
    if(EXISTS ${KM4_PROJECT_DIR}/asdk/image/km0_km4_app.bin)
        execute_process(
            COMMAND ${COMPRESSTOOL} ${KM4_PROJECT_DIR}/asdk/image/km0_km4_app.bin
        )
        execute_process(
            COMMAND ${COMPRESSTOOL} ${KM4_PROJECT_DIR}/asdk/image/km0_km4_app_ns.bin
        )
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E copy ${KM4_PROJECT_DIR}/asdk/image/km0_km4_app_compress.bin ${KM4_PROJECT_DIR}/asdk/image/tmp_app.bin
        )
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E copy ${KM4_PROJECT_DIR}/asdk/image/km0_km4_app_ns_compress.bin ${KM4_PROJECT_DIR}/asdk/image/tmp_app_ns.bin
        )
    endif()
else()
    if(EXISTS ${KM4_PROJECT_DIR}/asdk/image/km0_km4_app.bin)
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E copy ${KM4_PROJECT_DIR}/asdk/image/km0_km4_app.bin ${KM4_PROJECT_DIR}/asdk/image/tmp_app.bin
        )
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E copy ${KM4_PROJECT_DIR}/asdk/image/km0_km4_app_ns.bin ${KM4_PROJECT_DIR}/asdk/image/tmp_app_ns.bin
        )
    endif()
endif()

if(CONFIG_UPGRADE_BOOTLOADER)
    if(EXISTS ${KM4_PROJECT_DIR}/asdk/image/tmp_app.bin)
        execute_process(
            COMMAND ${OTAPREPENDTOOL} ${KM4_PROJECT_DIR}/asdk/image/tmp_app.bin ${KM4_PROJECT_DIR}/asdk/image/km4_boot_all.bin
        )
        execute_process(
            COMMAND ${OTAPREPENDTOOL} ${KM4_PROJECT_DIR}/asdk/image/tmp_app_ns.bin ${KM4_PROJECT_DIR}/asdk/image/km4_boot_all_ns.bin
        )
    endif()
else()
    if(EXISTS ${KM4_PROJECT_DIR}/asdk/image/tmp_app.bin)
        execute_process(
            COMMAND ${OTAPREPENDTOOL} ${KM4_PROJECT_DIR}/asdk/image/tmp_app.bin
        )
        execute_process(
            COMMAND ${OTAPREPENDTOOL} ${KM4_PROJECT_DIR}/asdk/image/tmp_app_ns.bin
        )
    endif()
endif()

execute_process(
    COMMAND ${CMAKE_COMMAND} -E remove ${KM4_PROJECT_DIR}/asdk/image/tmp_app.bin
)
execute_process(
    COMMAND ${CMAKE_COMMAND} -E remove ${KM4_PROJECT_DIR}/asdk/image/tmp_app_ns.bin
)
message("========== Image manipulating end ==========")
if (CONFIG_MP_INCLUDED)
    set(APP_ALL ${KM4_PROJECT_DIR}/asdk/image_mp/km0_km4_app_mp.bin)
else()
    set(APP_ALL	${KM4_PROJECT_DIR}/asdk/image/km0_km4_app.bin)
endif()
set(OTA_ALL	${KM4_PROJECT_DIR}/asdk/image/ota_all.bin)

if(EXISTS ${APP_ALL})
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy ${APP_ALL} ${FINAL_IMAGE_DIR}
    )
endif()

if(EXISTS ${OTA_ALL})
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy ${OTA_ALL} ${FINAL_IMAGE_DIR}
    )
endif()

if(NOT CONFIG_AMEBA_RLS)
    message("========== Image analyze start ==========")
    execute_process(
        COMMAND ${CODE_ANALYZE_PYTHON} ${ANALYZE_MP_IMG} ${DAILY_BUILD}
        WORKING_DIRECTORY ${c_MCU_PROJECT_DIR}/asdk
    )
    execute_process(
        COMMAND ${STATIC_ANALYZE_PYTHON} ${DAILY_BUILD}
        WORKING_DIRECTORY ${c_MCU_PROJECT_DIR}/asdk
    )
    message("========== Image analyze end ==========")
endif()

