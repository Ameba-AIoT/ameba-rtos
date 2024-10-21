#Note: Previously defined variables cannot be used directly in this file
#unless passed through -D
include(${CMAKE_FILES_DIR}/axf2bin.cmake)

if(CONFIG_MP_SHRINK)
    execute_process(COMMAND ${CMAKE_OBJCOPY} -j .boot.entry -j .sram_only.text.data -j .sram_image2.text.data -j .xip_image2.text -j .psram_image2.text.data -Obinary ${IMAGE_TARGET_FOLDER}/target_pure_img2.axf ${IMAGE_TARGET_FOLDER}/sram_2.bin)
else()
    execute_process(
        COMMAND ${CMAKE_OBJCOPY} -j .boot.entry -Obinary ${IMAGE_TARGET_FOLDER}/target_pure_img2.axf ${IMAGE_TARGET_FOLDER}/boot.bin
        COMMAND ${CMAKE_OBJCOPY} -j .sram_only.text.data -Obinary ${IMAGE_TARGET_FOLDER}/target_pure_img2.axf ${IMAGE_TARGET_FOLDER}/sram_only.bin
        COMMAND ${CMAKE_OBJCOPY} -j .sram_image2.text.data -Obinary ${IMAGE_TARGET_FOLDER}/target_pure_img2.axf ${IMAGE_TARGET_FOLDER}/sram_2.bin
        COMMAND ${CMAKE_OBJCOPY} -j .xip_image2.text -Obinary ${IMAGE_TARGET_FOLDER}/target_pure_img2.axf ${IMAGE_TARGET_FOLDER}/xip_image2.bin
        COMMAND ${CMAKE_OBJCOPY} -j .psram_image2.text.data -Obinary ${IMAGE_TARGET_FOLDER}/target_pure_img2.axf ${IMAGE_TARGET_FOLDER}/psram_2.bin
        COMMAND ${CMAKE_OBJCOPY} -j .ram_retention.text -j .ram_retention.entry -Obinary ${IMAGE_TARGET_FOLDER}/target_pure_img2.axf ${IMAGE_TARGET_FOLDER}/ram_retention.bin
    )
endif()


if(CONFIG_BT)
    execute_process(COMMAND ${CMAKE_OBJCOPY} -j .bluetooth_trace.text -Obinary ${IMAGE_TARGET_FOLDER}/target_pure_img2.axf ${IMAGE_TARGET_FOLDER}/APP.trace)
endif()

execute_process(
    COMMAND ${CMAKE_OBJCOPY} -j .coex_trace.text -Obinary ${IMAGE_TARGET_FOLDER}/target_pure_img2.axf ${IMAGE_TARGET_FOLDER}/COEX.trace
)

message( "========== Image Info HEX ==========")
execute_process(COMMAND ${CMAKE_SIZE} -A --radix=16 ${IMAGE_TARGET_FOLDER}/target_img2.axf)
execute_process(COMMAND ${CMAKE_SIZE} -t --radix=16 ${IMAGE_TARGET_FOLDER}/target_img2.axf)
message("========== Image Info HEX ==========")


message( "========== Image Info DEC ==========")
execute_process(COMMAND ${CMAKE_SIZE} -A --radix=10 ${IMAGE_TARGET_FOLDER}/target_img2.axf)
execute_process(COMMAND ${CMAKE_SIZE} -t --radix=10 ${IMAGE_TARGET_FOLDER}/target_img2.axf)
message( "========== Image Info DEC ==========")


message( "========== Image manipulating start ==========")

if(CONFIG_MP_SHRINK)
    execute_process(COMMAND ${PREPENDTOOL} ${IMAGE_TARGET_FOLDER}/sram_2.bin  __boot_text_start__  ${IMAGE_TARGET_FOLDER}/target_img2.map)
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E cat ${IMAGE_TARGET_FOLDER}/sram_2_prepend.bin
        OUTPUT_FILE ${IMAGE_TARGET_FOLDER}/kr4_image2_all_shrink.bin
    )
    execute_process(COMMAND ${IMAGETOOL} ${IMAGE_TARGET_FOLDER}/kr4_image2_all_shrink.bin ${BUILD_TYPE} ${DSP_IMAGE_TARGET_DIR})
else()
    execute_process(
        COMMAND ${PADTOOL} ${IMAGE_TARGET_FOLDER}/boot.bin  32
        COMMAND ${PADTOOL} ${IMAGE_TARGET_FOLDER}/sram_only.bin  32
        COMMAND ${PADTOOL} ${IMAGE_TARGET_FOLDER}/sram_2.bin  32
        COMMAND ${PADTOOL} ${IMAGE_TARGET_FOLDER}/psram_2.bin  32
        COMMAND ${PADTOOL} ${IMAGE_TARGET_FOLDER}/xip_image2.bin  32
    )
    execute_process(
        COMMAND ${PREPENDTOOL} ${IMAGE_TARGET_FOLDER}/boot.bin  __boot_text_start__  ${IMAGE_TARGET_FOLDER}/target_img2.map
        COMMAND ${PREPENDTOOL} ${IMAGE_TARGET_FOLDER}/sram_only.bin __sram_only_start__  ${IMAGE_TARGET_FOLDER}/target_img2.map
        COMMAND ${PREPENDTOOL} ${IMAGE_TARGET_FOLDER}/sram_2.bin  __sram_image2_start__  ${IMAGE_TARGET_FOLDER}/target_img2.map
        COMMAND ${PREPENDTOOL} ${IMAGE_TARGET_FOLDER}/psram_2.bin  __psram_image2_start__  ${IMAGE_TARGET_FOLDER}/target_img2.map
        COMMAND ${PREPENDTOOL} ${IMAGE_TARGET_FOLDER}/xip_image2.bin  __flash_text_start__  ${IMAGE_TARGET_FOLDER}/target_img2.map
    )
    # pad to 4K-aligned for RSIP/MMU address 4K-alignment restriction
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E cat ${IMAGE_TARGET_FOLDER}/xip_image2_prepend.bin ${IMAGE_TARGET_FOLDER}/psram_2_prepend.bin ${IMAGE_TARGET_FOLDER}/sram_2_prepend.bin ${IMAGE_TARGET_FOLDER}/boot_prepend.bin ${IMAGE_TARGET_FOLDER}/sram_only_prepend.bin
        OUTPUT_FILE ${IMAGE_TARGET_FOLDER}/kr4_image2_all.bin
    )
    execute_process(COMMAND ${PADTOOL} ${IMAGE_TARGET_FOLDER}/kr4_image2_all.bin 4096)
    execute_process(COMMAND ${IMAGETOOL} ${IMAGE_TARGET_FOLDER}/kr4_image2_all.bin ${BUILD_TYPE} ${DSP_IMAGE_TARGET_DIR})
endif()

if(CONFIG_FATFS_WITHIN_APP_IMG)
    if(EXISTS ${KM4_BUILDDIR}/asdk/image/kr4_km4_app.bin AND EXISTS ${BASEDIR}/amebalite_gcc_project/fatfs.bin)
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E rename ${KM4_BUILDDIR}/asdk/image/kr4_km4_app.bin ${KM4_BUILDDIR}/asdk/image/kr4_km4_app_tmp.bin
            COMMAND ${CMAKE_COMMAND} -E rename ${KM4_BUILDDIR}/asdk/image/kr4_km4_app_ns.bin ${KM4_BUILDDIR}/asdk/image/kr4_km4_app_ns_tmp.bin
        )
        execute_process(
            COMMAND ${PADTOOL} ${KM4_BUILDDIR}/asdk/image/kr4_km4_app_tmp.bin 4096
            COMMAND ${PADTOOL} ${KM4_BUILDDIR}/asdk/image/kr4_km4_app_ns_tmp.bin 4096
            COMMAND ${PREPENDTOOL} ${BASEDIR}/amebalite_gcc_project/fatfs.bin  VFS1_FLASH_BASE_ADDR  ${AP_IMAGEDIR}/target_img2.map
        )
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E cat ${KM4_BUILDDIR}/asdk/image/kr4_km4_app_tmp.bin ${BASEDIR}/amebalite_gcc_project/fatfs_prepend.bin
            OUTPUT_FILE ${KM4_BUILDDIR}/asdk/image/kr4_km4_app.bin
        )
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E cat ${KM4_BUILDDIR}/asdk/image/kr4_km4_app_ns_tmp.bin ${BASEDIR}/amebalite_gcc_project/fatfs_prepend.bin
            OUTPUT_FILE ${KM4_BUILDDIR}/asdk/image/kr4_km4_app_ns.bin
        )
    else()
        if(EXISTS ${KM4_BUILDDIR}/asdk/image/kr4_km4_dsp_app.bin AND EXISTS ${BASEDIR}/amebalite_gcc_project/fatfs.bin)
            execute_process(
                COMMAND ${CMAKE_COMMAND} -E rename ${KM4_BUILDDIR}/asdk/image/kr4_km4_dsp_app.bin ${KM4_BUILDDIR}/asdk/image/kr4_km4_dsp_app_tmp.bin
                COMMAND ${CMAKE_COMMAND} -E rename ${KM4_BUILDDIR}/asdk/image/kr4_km4_dsp_app_ns.bin ${KM4_BUILDDIR}/asdk/image/kr4_km4_dsp_app_ns_tmp.bin
            )
            execute_process(
                COMMAND ${PADTOOL} ${KM4_BUILDDIR}/asdk/image/kr4_km4_dsp_app_tmp.bin 4096
                COMMAND ${PADTOOL} ${KM4_BUILDDIR}/asdk/image/kr4_km4_dsp_app_ns_tmp.bin 4096
                COMMAND ${PREPENDTOOL} ${BASEDIR}/amebalite_gcc_project/fatfs.bin  VFS1_FLASH_BASE_ADDR  ${AP_IMAGEDIR}/target_img2.map
            )
            execute_process(
                COMMAND ${CMAKE_COMMAND} -E cat ${KM4_BUILDDIR}/asdk/image/kr4_km4_dsp_app_tmp.bin ${BASEDIR}/amebalite_gcc_project/fatfs_prepend.bin
                OUTPUT_FILE ${KM4_BUILDDIR}/asdk/image/kr4_km4_dsp_app.bin
            )
            execute_process(
                COMMAND ${CMAKE_COMMAND} -E cat ${KM4_BUILDDIR}/asdk/image/kr4_km4_dsp_app_ns_tmp.bin ${BASEDIR}/amebalite_gcc_project/fatfs_prepend.bin
                OUTPUT_FILE ${KM4_BUILDDIR}/asdk/image/kr4_km4_dsp_app_ns.bin
            )
        endif()
    endif()
endif()

if(CONFIG_COMPRESS_OTA_IMG)
    if(EXISTS ${KM4_BUILDDIR}/asdk/image/kr4_km4_app.bin)
        execute_process(
            COMMAND ${COMPRESSTOOL} ${KM4_BUILDDIR}/asdk/image/kr4_km4_app.bin
            COMMAND ${COMPRESSTOOL} ${KM4_BUILDDIR}/asdk/image/kr4_km4_app_ns.bin
        )
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E copy ${KM4_BUILDDIR}/asdk/image/kr4_km4_app_compress.bin ${KM4_BUILDDIR}/asdk/image/tmp_app.bin
            COMMAND ${CMAKE_COMMAND} -E copy ${KM4_BUILDDIR}/asdk/image/kr4_km4_app_ns_compress.bin ${KM4_BUILDDIR}/asdk/image/tmp_app_ns.bin
        )
    else()
        if(EXISTS ${KM4_BUILDDIR}/asdk/image/kr4_km4_dsp_app.bin)
            execute_process(
                COMMAND ${COMPRESSTOOL} ${KM4_BUILDDIR}/asdk/image/kr4_km4_dsp_app.bin
                COMMAND ${COMPRESSTOOL} ${KM4_BUILDDIR}/asdk/image/kr4_km4_dsp_app_ns.bin
            )
            execute_process(
                COMMAND ${CMAKE_COMMAND} -E copy ${KM4_BUILDDIR}/asdk/image/kr4_km4_dsp_app_compress.bin ${KM4_BUILDDIR}/asdk/image/tmp_app.bin
                COMMAND ${CMAKE_COMMAND} -E copy ${KM4_BUILDDIR}/asdk/image/kr4_km4_dsp_app_ns_compress.bin ${KM4_BUILDDIR}/asdk/image/tmp_app_ns.bin
            )
        endif()
    endif()
else()
    if(EXISTS ${KM4_BUILDDIR}/asdk/image/kr4_km4_app.bin)
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E copy ${KM4_BUILDDIR}/asdk/image/kr4_km4_app.bin ${KM4_BUILDDIR}/asdk/image/tmp_app.bin
            COMMAND ${CMAKE_COMMAND} -E copy ${KM4_BUILDDIR}/asdk/image/kr4_km4_app_ns.bin ${KM4_BUILDDIR}/asdk/image/tmp_app_ns.bin
        )
    else()
        if(EXISTS ${KM4_BUILDDIR}/asdk/image/kr4_km4_dsp_app.bin)
            execute_process(
                COMMAND ${CMAKE_COMMAND} -E copy ${KM4_BUILDDIR}/asdk/image/kr4_km4_dsp_app.bin ${KM4_BUILDDIR}/asdk/image/tmp_app.bin
                COMMAND ${CMAKE_COMMAND} -E copy ${KM4_BUILDDIR}/asdk/image/kr4_km4_dsp_app_ns.bin ${KM4_BUILDDIR}/asdk/image/tmp_app_ns.bin
            )
        endif()
    endif()
endif()

if(CONFIG_UPGRADE_BOOTLOADER)
    if(EXISTS ${KM4_BUILDDIR}/asdk/image/tmp_app.bin)
        execute_process(
            COMMAND ${OTAPREPENDTOOL} ${KM4_BUILDDIR}/asdk/image/tmp_app.bin ${KM4_BUILDDIR}/asdk/image/km4_boot_all.bin
            COMMAND ${OTAPREPENDTOOL} ${KM4_BUILDDIR}/asdk/image/tmp_app_ns.bin ${KM4_BUILDDIR}/asdk/image/km4_boot_all_ns.bin
        )
    endif()
else()
    if(EXISTS ${KM4_BUILDDIR}/asdk/image/tmp_app.bin)
        execute_process(
            COMMAND ${OTAPREPENDTOOL} ${KM4_BUILDDIR}/asdk/image/tmp_app.bin
            COMMAND ${OTAPREPENDTOOL} ${KM4_BUILDDIR}/asdk/image/tmp_app_ns.bin
        )
    endif()
endif()

execute_process(
    COMMAND ${CMAKE_COMMAND} -E remove ${KM4_BUILDDIR}/asdk/image/tmp_app.bin
    COMMAND ${CMAKE_COMMAND} -E remove ${KM4_BUILDDIR}/asdk/image/tmp_app_ns.bin
)
message("========== Image manipulating end ==========")

if (CONFIG_MP_INCLUDED)
    set(APP_ALL ${KM4_BUILDDIR}/asdk/image_mp/kr4_km4_app_mp.bin)
elseif(CONFIG_DSP_WITHIN_APP_IMG)
    set(APP_ALL ${KM4_BUILDDIR}/asdk/image/kr4_km4_dsp_app.bin)
else()
    set(APP_ALL	${KM4_BUILDDIR}/asdk/image/kr4_km4_app.bin)
endif()
set(OTA_ALL	${KM4_BUILDDIR}/asdk/image/ota_all.bin)

if(EXISTS ${APP_ALL})
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${APP_ALL} ${IMAGE_TARGET_FOLDER}/../../..)
endif()

if(EXISTS ${OTA_ALL})
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${OTA_ALL} ${IMAGE_TARGET_FOLDER}/../../..)
endif()

if(NOT AMEBA_RLS)
    message("========== Image analyze start ==========")
    execute_process(
        COMMAND ${CODE_ANALYZE_PYTHON} ${ANALYZE_MP_IMG} ${DAILY_BUILD}
        WORKING_DIRECTORY ${PROJECTDIR}/vsdk
    )
    execute_process(
        COMMAND ${STATIC_ANALYZE_PYTHON} ${DAILY_BUILD}
        WORKING_DIRECTORY ${PROJECTDIR}/vsdk
    )
    message("========== Image analyze end ==========")
endif()
