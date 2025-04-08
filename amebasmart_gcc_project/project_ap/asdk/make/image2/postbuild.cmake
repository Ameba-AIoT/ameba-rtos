#Note: Previously defined variables cannot be used directly in this file
#unless passed through -D
include(${c_CMAKE_FILES_DIR}/axf2bin.cmake)
include(${c_CMAKE_FILES_DIR}/utility.cmake)
include(${c_CMAKE_FILES_DIR}/global_define.cmake)
import_kconfig("CONFIG" ${c_MCU_KCONFIG_FILE})
set(v_USER_CUSTOM_LOG_PREFIX "AP_POSTBUILD")


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
        COMMAND ${CMAKE_OBJCOPY} -j .code -j .ARM.extab -j .ARM.exidx -j .ctors -j .dtors -j .preinit_array -j .init_array -j .fini_array -j .data -j .mmu_tbl -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/ram_2.bin
        COMMAND ${CMAKE_OBJCOPY} -j .xip_image2.text -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/xip_image2.bin
    )
else()
    execute_process(
        COMMAND ${CMAKE_OBJCOPY} -j .code -j .data -j .mmu_tbl -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/ram_2.bin
        COMMAND ${CMAKE_OBJCOPY} -j .xip_image2.text -j .ARM.extab -j .ARM.exidx -j .ctors -j .dtors -j .preinit_array -j .init_array -j .fini_array -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/xip_image2.bin
    )
endif()


if(CONFIG_BT)
    execute_process(
        COMMAND ${CMAKE_OBJCOPY} -j .bluetooth_trace.text -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/APP.trace
    )
endif()


execute_process(
    COMMAND ${CMAKE_COMMAND} -E cat ${c_SDK_IMAGE_TARGET_DIR}/ram_2.bin
    OUTPUT_FILE ${c_SDK_IMAGE_TARGET_DIR}/ca32_image2_all.bin
)

message( "========== Image manipulating start ==========")

# Call make -C atf image to generate fip.bin after ca32_image2_all.bin is generated
# TODO: need to catch the error
execute_process(
    COMMAND ${CMAKE_COMMAND} -E echo "Building atf img"
    COMMAND ${CMAKE_COMMAND} -E env make -j CROSS_COMPILE=${CROSS_COMPILE} PROJECT_DIR=${c_SOC_PROJECT_DIR}/project_ap BUILD_BASE=${BUILD_BASE} -C ${c_BASEDIR}/component/soc/${c_SOC_TYPE}/atf image
    COMMAND ${CMAKE_COMMAND} -E copy ${BUILD_BASE}/project_ap/asdk/make/atf/bl1/bl1.dump ${IMAGE_TARGET_FOLDER}/bl1.dump
    COMMAND ${CMAKE_COMMAND} -E copy ${BUILD_BASE}/project_ap/asdk/make/atf/bl2/bl2.dump ${IMAGE_TARGET_FOLDER}/bl2.dump
    COMMAND ${CMAKE_COMMAND} -E copy ${BUILD_BASE}/project_ap/asdk/make/atf/bl32/bl32.dump ${IMAGE_TARGET_FOLDER}/bl32.dump
    COMMAND ${CMAKE_COMMAND} -E copy ${BUILD_BASE}/project_ap/asdk/make/atf/bl1/bl1.map ${IMAGE_TARGET_FOLDER}/bl1.map
    COMMAND ${CMAKE_COMMAND} -E copy ${BUILD_BASE}/project_ap/asdk/make/atf/bl2/bl2.map ${IMAGE_TARGET_FOLDER}/bl2.map
    COMMAND ${CMAKE_COMMAND} -E copy ${BUILD_BASE}/project_ap/asdk/make/atf/bl32/bl32.map ${IMAGE_TARGET_FOLDER}/bl32.map
    WORKING_DIRECTORY ${c_BASEDIR}/component/soc/${c_SOC_TYPE}/atf
)

execute_process(
    COMMAND ${PADTOOL} ${c_SDK_IMAGE_TARGET_DIR}/bl1_sram.bin 32
)
execute_process(
    COMMAND ${PADTOOL} ${c_SDK_IMAGE_TARGET_DIR}/bl1.bin 32
)
execute_process(
    COMMAND ${PADTOOL} ${c_SDK_IMAGE_TARGET_DIR}/fip.bin 32
)
execute_process(
    COMMAND ${PADTOOL} ${c_SDK_IMAGE_TARGET_DIR}/xip_image2.bin 32
)


execute_process(
    COMMAND ${PREPENDTOOL} ${c_SDK_IMAGE_TARGET_DIR}/xip_image2.bin  __flash_text_start__  ${c_SDK_IMAGE_TARGET_DIR}/target_img2.map
)
execute_process(
    COMMAND ${PREPENDTOOL} ${c_SDK_IMAGE_TARGET_DIR}/bl1_sram.bin  __ca32_bl1_sram_start__  ${c_SDK_IMAGE_TARGET_DIR}/target_img2.map
)
execute_process(
    COMMAND ${PREPENDTOOL} ${c_SDK_IMAGE_TARGET_DIR}/bl1.bin  __ca32_bl1_dram_start__  ${c_SDK_IMAGE_TARGET_DIR}/target_img2.map
)
execute_process(
    COMMAND ${PREPENDTOOL} ${c_SDK_IMAGE_TARGET_DIR}/fip.bin  __ca32_fip_dram_start__  ${c_SDK_IMAGE_TARGET_DIR}/target_img2.map
)
execute_process(
    COMMAND ${CMAKE_COMMAND} -E cat ${c_SDK_IMAGE_TARGET_DIR}/xip_image2_prepend.bin ${c_SDK_IMAGE_TARGET_DIR}/bl1_sram_prepend.bin ${c_SDK_IMAGE_TARGET_DIR}/bl1_prepend.bin ${c_SDK_IMAGE_TARGET_DIR}/fip_prepend.bin
    OUTPUT_FILE ${c_SDK_IMAGE_TARGET_DIR}/ap_image_all.bin
)
execute_process(
    COMMAND ${IMAGETOOL} ${c_SDK_IMAGE_TARGET_DIR}/ap_image_all.bin ${BUILD_TYPE}
    WORKING_DIRECTORY ${c_MCU_PROJECT_DIR}/..
)

if(CONFIG_DYNAMIC_APP_LOAD_EN)
    execute_process(
        COMMAND python ${c_BASEDIR}/component/dynamic_app_load/elf2tinf/imagetool.py imagetool ${c_BASEDIR}/component/dynamic_app_load/tinfbin/
    )

    if(EXISTS ${c_BASEDIR}/component/dynamic_app_load/tinfbin/dynamic_app.bin)
        execute_process(
            COMMAND ${PREPENDTOOL} ${c_BASEDIR}/component/dynamic_app_load/tinfbin/dynamic_app.bin  __dram_dynamic_app_text_start__  ${c_SDK_IMAGE_TARGET_DIR}/target_img2.map
        )

        execute_process(
            COMMAND ${CMAKE_COMMAND} -E rename ${c_SDK_IMAGE_TARGET_DIR}/ap_image_all.bin ${c_SDK_IMAGE_TARGET_DIR}/ap_image_all_tmp.bin
        )

        execute_process(
            COMMAND ${CMAKE_COMMAND} -E cat ${c_SDK_IMAGE_TARGET_DIR}/ap_image_all_tmp.bin ${c_BASEDIR}/component/dynamic_app_load/tinfbin/dynamic_app_prepend.bin
            OUTPUT_FILE ${c_SDK_IMAGE_TARGET_DIR}/ap_image_all.bin
        )

        execute_process(
            COMMAND ${CMAKE_COMMAND} -E remove ${c_SDK_IMAGE_TARGET_DIR}/ap_image_all_tmp.bin
        )

        if(EXISTS ${c_SDK_IMAGE_TARGET_DIR}/ap_image_all_en.bin)
            execute_process(
                COMMAND ${CMAKE_COMMAND} -E rename ${c_SDK_IMAGE_TARGET_DIR}/ap_image_all_en.bin ${c_SDK_IMAGE_TARGET_DIR}/ap_image_all_en_tmp.bin
            )

            execute_process(
                COMMAND ${CMAKE_COMMAND} -E cat ${c_SDK_IMAGE_TARGET_DIR}/ap_image_all_en_tmp.bin ${c_BASEDIR}/component/dynamic_app_load/tinfbin/dynamic_app_prepend.bin
                OUTPUT_FILE ${c_SDK_IMAGE_TARGET_DIR}/ap_image_all_en.bin
            )

            execute_process(
                COMMAND ${CMAKE_COMMAND} -E remove ${c_SDK_IMAGE_TARGET_DIR}/ap_image_all_en_tmp.bin
            )
        endif()
    endif()

endif()

if(CONFIG_FATFS_WITHIN_APP_IMG)
    if(EXISTS ${KM4_PROJECT_DIR}/asdk/image/km0_km4_ca32_app.bin AND EXISTS ${c_BASEDIR}/{c_SOC_PROJECT_DIR}/fatfs.bin)

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
            COMMAND ${PREPENDTOOL} ${c_BASEDIR}/{c_SOC_PROJECT_DIR}/fatfs.bin  VFS1_FLASH_BASE_ADDR  ${c_SDK_IMAGE_TARGET_DIR}/target_img2.map
        )

        execute_process(
            COMMAND ${CMAKE_COMMAND} -E cat ${KM4_PROJECT_DIR}/asdk/image/km0_km4_ca32_app_tmp.bin ${c_BASEDIR}/{c_SOC_PROJECT_DIR}/fatfs_prepend.bin
            OUTPUT_FILE ${KM4_PROJECT_DIR}/asdk/image/km0_km4_ca32_app.bin
        )

        execute_process(
            COMMAND ${CMAKE_COMMAND} -E cat ${KM4_PROJECT_DIR}/asdk/image/km0_km4_ca32_app_ns_tmp.bin ${c_BASEDIR}/{c_SOC_PROJECT_DIR}/fatfs_prepend.bin
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
        WORKING_DIRECTORY ${c_MCU_PROJECT_DIR}/asdk
    )
    execute_process(
        COMMAND ${STATIC_ANALYZE_PYTHON} ${DAILY_BUILD}
        WORKING_DIRECTORY ${c_MCU_PROJECT_DIR}/asdk
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
