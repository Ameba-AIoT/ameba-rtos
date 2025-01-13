#Note: Previously defined variables cannot be used directly in this file
#unless passed through -D
include(${CMAKE_FILES_DIR}/axf2bin.cmake)
include(${CMAKE_FILES_DIR}/utility.cmake)
include(${CMAKE_FILES_DIR}/global_define.cmake)
set(t_USER_CUSTOM_LOG_PREFIX "AP_POSTBUILD")


file(READ ${IMAGE_TARGET_FOLDER}/target_img2.map content)
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
        COMMAND ${CMAKE_OBJCOPY} -j .code -j .ARM.extab -j .ARM.exidx -j .ctors -j .dtors -j .preinit_array -j .init_array -j .fini_array -j .data -j .mmu_tbl -Obinary ${IMAGE_TARGET_FOLDER}/target_pure_img2.axf ${IMAGE_TARGET_FOLDER}/ram_2.bin
        COMMAND ${CMAKE_OBJCOPY} -j .xip_image2.text -Obinary ${IMAGE_TARGET_FOLDER}/target_pure_img2.axf ${IMAGE_TARGET_FOLDER}/xip_image2.bin
    )
else()
    execute_process(
        COMMAND ${CMAKE_OBJCOPY} -j .code -j .data -j .mmu_tbl -Obinary ${IMAGE_TARGET_FOLDER}/target_pure_img2.axf ${IMAGE_TARGET_FOLDER}/ram_2.bin
        COMMAND ${CMAKE_OBJCOPY} -j .xip_image2.text -j .ARM.extab -j .ARM.exidx -j .ctors -j .dtors -j .preinit_array -j .init_array -j .fini_array -Obinary ${IMAGE_TARGET_FOLDER}/target_pure_img2.axf ${IMAGE_TARGET_FOLDER}/xip_image2.bin
    )
endif()


if(CONFIG_BT)
    execute_process(
        COMMAND ${CMAKE_OBJCOPY} -j .bluetooth_trace.text -Obinary ${IMAGE_TARGET_FOLDER}/target_pure_img2.axf ${IMAGE_TARGET_FOLDER}/APP.trace
    )
endif()


execute_process(
    COMMAND ${CMAKE_COMMAND} -E cat ${IMAGE_TARGET_FOLDER}/ram_2.bin
    OUTPUT_FILE ${IMAGE_TARGET_FOLDER}/ca32_image2_all.bin
)

message( "========== Image manipulating start ==========")

execute_process(
    COMMAND ${CMAKE_COMMAND} -E echo "Building atf img"
    COMMAND ${CMAKE_COMMAND} -E env make -j TOOLCHAIN_DIR=${TOOLCHAIN_DIR} ASDK_VER=${ASDK_VER} TOOLCHAINVER=${TOOLCHAINVER} -C ${c_BASEDIR}/component/soc/amebasmart/atf image
    WORKING_DIRECTORY ${c_BASEDIR}/component/soc/amebasmart/atf
)

execute_process(
    COMMAND ${PADTOOL} ${IMAGE_TARGET_FOLDER}/bl1_sram.bin 32
)
execute_process(
    COMMAND ${PADTOOL} ${IMAGE_TARGET_FOLDER}/bl1.bin 32
)
execute_process(
    COMMAND ${PADTOOL} ${IMAGE_TARGET_FOLDER}/fip.bin 32
)
execute_process(
    COMMAND ${PADTOOL} ${IMAGE_TARGET_FOLDER}/xip_image2.bin 32
)


execute_process(
    COMMAND ${PREPENDTOOL} ${IMAGE_TARGET_FOLDER}/xip_image2.bin  __flash_text_start__  ${IMAGE_TARGET_FOLDER}/target_img2.map
)
execute_process(
    COMMAND ${PREPENDTOOL} ${IMAGE_TARGET_FOLDER}/bl1_sram.bin  __ca32_bl1_sram_start__  ${IMAGE_TARGET_FOLDER}/target_img2.map
)
execute_process(
    COMMAND ${PREPENDTOOL} ${IMAGE_TARGET_FOLDER}/bl1.bin  __ca32_bl1_dram_start__  ${IMAGE_TARGET_FOLDER}/target_img2.map
)
execute_process(
    COMMAND ${PREPENDTOOL} ${IMAGE_TARGET_FOLDER}/fip.bin  __ca32_fip_dram_start__  ${IMAGE_TARGET_FOLDER}/target_img2.map
)
execute_process(
    COMMAND ${CMAKE_COMMAND} -E cat ${IMAGE_TARGET_FOLDER}/xip_image2_prepend.bin ${IMAGE_TARGET_FOLDER}/bl1_sram_prepend.bin ${IMAGE_TARGET_FOLDER}/bl1_prepend.bin ${IMAGE_TARGET_FOLDER}/fip_prepend.bin
    OUTPUT_FILE ${IMAGE_TARGET_FOLDER}/ap_image_all.bin
)
execute_process(
    COMMAND ${IMAGETOOL} ${IMAGE_TARGET_FOLDER}/ap_image_all.bin ${BUILD_TYPE}
    WORKING_DIRECTORY ${PROJECTDIR}/..
)

if(CONFIG_DYNAMIC_APP_LOAD_EN)
    execute_process(
        COMMAND python ${c_BASEDIR}/component/dynamic_app_load/elf2tinf/imagetool.py imagetool ${c_BASEDIR}/component/dynamic_app_load/tinfbin/
    )

    if(EXISTS ${c_BASEDIR}/component/dynamic_app_load/tinfbin/dynamic_app.bin)
        execute_process(
            COMMAND ${PREPENDTOOL} ${c_BASEDIR}/component/dynamic_app_load/tinfbin/dynamic_app.bin  __dram_dynamic_app_text_start__  ${IMAGE_TARGET_FOLDER}/target_img2.map
        )

        execute_process(
            COMMAND ${CMAKE_COMMAND} -E rename ${IMAGE_TARGET_FOLDER}/ap_image_all.bin ${IMAGE_TARGET_FOLDER}/ap_image_all_tmp.bin
        )

        execute_process(
            COMMAND ${CMAKE_COMMAND} -E cat ${IMAGE_TARGET_FOLDER}/ap_image_all_tmp.bin ${c_BASEDIR}/component/dynamic_app_load/tinfbin/dynamic_app_prepend.bin
            OUTPUT_FILE ${IMAGE_TARGET_FOLDER}/ap_image_all.bin
        )

        execute_process(
            COMMAND ${CMAKE_COMMAND} -E remove ${IMAGE_TARGET_FOLDER}/ap_image_all_tmp.bin
        )

        execute_process(
            COMMAND ${CMAKE_COMMAND} -E rename ${IMAGE_TARGET_FOLDER}/ap_image_all_en.bin ${IMAGE_TARGET_FOLDER}/ap_image_all_en_tmp.bin
        )

        execute_process(
            COMMAND ${CMAKE_COMMAND} -E cat ${IMAGE_TARGET_FOLDER}/ap_image_all_en_tmp.bin ${c_BASEDIR}/component/dynamic_app_load/tinfbin/dynamic_app_prepend.bin
            OUTPUT_FILE ${IMAGE_TARGET_FOLDER}/ap_image_all_en.bin
        )

        execute_process(
            COMMAND ${CMAKE_COMMAND} -E remove ${IMAGE_TARGET_FOLDER}/ap_image_all_en_tmp.bin
        )
    endif()

endif()

if(CONFIG_FATFS_WITHIN_APP_IMG)
    if(EXISTS ${KM4_PROJECT_DIR}/asdk/image/km0_km4_ca32_app.bin AND EXISTS ${c_BASEDIR}/amebasmart_gcc_project/fatfs.bin)

        execute_process(
            COMMAND ${CMAKE_COMMAND} -E rename ${KM4_PROJECT_DIR}/asdk/image/km0_km4_ca32_app.bin ${KM4_PROJECT_DIR}/asdk/image/km0_km4_ca32_app_tmp.bin
        )

        execute_process(
            COMMAND ${CMAKE_COMMAND} -E rename ${KM4_PROJECT_DIR}/asdk/image/km0_km4_ca32_app_ns.bin ${KM4_PROJECT_DIR}/asdk/image/km0_km4_ca32_app_ns_tmp.bin
        )

        execute_process(
            COMMAND ${PADTOOL} ${KM4_PROJECT_DIR}/asdk/image/km0_km4_ca32_app_tmp.bin 4096
        )

        execute_process(
            COMMAND ${PADTOOL} ${KM4_PROJECT_DIR}/asdk/image/km0_km4_ca32_app_ns_tmp.bin 4096
        )

        execute_process(
            COMMAND ${PREPENDTOOL} ${c_BASEDIR}/amebasmart_gcc_project/fatfs.bin  VFS1_FLASH_BASE_ADDR  ${IMAGE_TARGET_FOLDER}/target_img2.map
        )

        execute_process(
            COMMAND ${CMAKE_COMMAND} -E cat ${KM4_PROJECT_DIR}/asdk/image/km0_km4_ca32_app_tmp.bin ${c_BASEDIR}/amebasmart_gcc_project/fatfs_prepend.bin
            OUTPUT_FILE ${KM4_PROJECT_DIR}/asdk/image/km0_km4_ca32_app.bin
        )

        execute_process(
            COMMAND ${CMAKE_COMMAND} -E cat ${KM4_PROJECT_DIR}/asdk/image/km0_km4_ca32_app_ns_tmp.bin ${c_BASEDIR}/amebasmart_gcc_project/fatfs_prepend.bin
            OUTPUT_FILE ${KM4_PROJECT_DIR}/asdk/image/km0_km4_ca32_app_ns.bin
        )
    endif()
endif()

if(CONFIG_COMPRESS_OTA_IMG)
    if(EXISTS ${KM4_PROJECT_DIR}/asdk/image/km0_km4_ca32_app.bin)
        execute_process(
            COMMAND ${COMPRESSTOOL} ${KM4_PROJECT_DIR}/asdk/image/km0_km4_ca32_app.bin
        )
        execute_process(
            COMMAND ${COMPRESSTOOL} ${KM4_PROJECT_DIR}/asdk/image/km0_km4_ca32_app.bin
        )
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E copy  ${KM4_PROJECT_DIR}/asdk/image/km0_km4_ca32_app_compress.bin  ${KM4_PROJECT_DIR}/asdk/image/tmp_app.bin
        )
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E copy  ${KM4_PROJECT_DIR}/asdk/image/km0_km4_ca32_app_ns_compress.bin  ${KM4_PROJECT_DIR}/asdk/image}/tmp_app_ns.bin
        )
    endif()
else()
    if(EXISTS ${KM4_PROJECT_DIR}/asdk/image/km0_km4_ca32_app.bin)
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E copy ${KM4_PROJECT_DIR}/asdk/image/km0_km4_ca32_app.bin ${KM4_PROJECT_DIR}/asdk/image/tmp_app.bin
        )
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E copy ${KM4_PROJECT_DIR}/asdk/image/km0_km4_ca32_app_ns.bin ${KM4_PROJECT_DIR}/asdk/image/tmp_app_ns.bin
        )
        unset(_tmp_bin_name)
    endif()
endif()

if(CONFIG_UPGRADE_BOOTLOADER)
    if(EXISTS ${KM4_PROJECT_DIR}/asdk/image/tmp_app.bin)
        execute_process(
            COMMAND ${OTAPREPENDTOOL} ${KM4_PROJECT_DIR}/asdk/image/tmp_app.bin ${KM4_PROJECT_DIR}/asdk/image/km4_boot_all.bin
        )
        execute_process(
            COMMAND ${OTAPREPENDTOOL} ${KM4_PROJECT_DIR}/asdk/image/tmp_app_ns.bin${KM4_PROJECT_DIR}/asdk/image/km4_boot_all_ns.bin
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
execute_process(
    COMMAND ${CMAKE_COMMAND} -E remove ${KM4_PROJECT_DIR}/*prepend.bin
)
message("========== Image manipulating end ==========")


if(NOT CONFIG_AMEBA_RLS)
    message("========== Image analyze start ==========")
    execute_process(
        COMMAND ${CODE_ANALYZE_PYTHON} ${ANALYZE_MP_IMG} ${DAILY_BUILD}
        WORKING_DIRECTORY ${PROJECTDIR}/asdk
    )
    execute_process(
        COMMAND ${STATIC_ANALYZE_PYTHON} ${DAILY_BUILD}
        WORKING_DIRECTORY ${PROJECTDIR}/asdk
    )
    message("========== Image analyze end ==========")
endif()

if(CONFIG_MP_INCLUDED)
    set(km4_boot_all ${KM4_PROJECT_DIR}/asdk/image_mp/km4_boot_all.bin)
    if(CONFIG_LINUX_FW_EN)
        set(app_all ${KM4_PROJECT_DIR}/asdk/image_mp/km0_km4_app_mp.bin)
    else()
        set(app_all ${KM4_PROJECT_DIR}/asdk/image_mp/km0_km4_ca32_app_mp.bin)
    endif()
else()
    set(km4_boot_all ${KM4_PROJECT_DIR}/asdk/image/km4_boot_all.bin)
    set(km4_boot_all_sec ${KM4_PROJECT_DIR}/asdk/image/km4_boot_all_sec.bin)

    if(CONFIG_LINUX_FW_EN)
        set(app_all ${KM4_PROJECT_DIR}/asdk/image/km0_km4_app.bin)
        set(km0_image2_all_sec ${KM4_PROJECT_DIR}/asdk/image/km0_km4_app_sec.bin)
    else()
        set(app_all ${KM4_PROJECT_DIR}/asdk/image/km0_km4_ca32_app.bin)
        set(km0_image2_all_sec  ${KM4_PROJECT_DIR}/asdk/image/km0_km4_ca32_app_sec.bin)
    endif()
endif()
set(ota_all  ${KM4_PROJECT_DIR}/asdk/image/ota_all.bin)

if(EXISTS ${km4_boot_all})
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy ${km4_boot_all} ${FINAL_IMAGE_DIR}
    )
endif()
if(EXISTS ${app_all})
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy ${app_all} ${FINAL_IMAGE_DIR}
    )
endif()
if(EXISTS ${ota_all})
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy ${ota_all} ${FINAL_IMAGE_DIR}
    )
endif()
if(NOT CONFIG_MP_INCLUDED)
    if(EXISTS ${km4_boot_all_sec})
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E copy ${km4_boot_all_sec} ${FINAL_IMAGE_DIR}
        )
    endif()
    if(EXISTS ${km0_image2_all_sec})
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E copy ${km0_image2_all_sec} ${FINAL_IMAGE_DIR}
        )
    endif()
endif()