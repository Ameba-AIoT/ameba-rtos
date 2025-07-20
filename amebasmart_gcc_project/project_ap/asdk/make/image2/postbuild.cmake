#Note: Previously defined variables cannot be used directly in this file
#unless passed through -D
cmake_minimum_required(VERSION 3.20.0)
include(${c_CMAKE_FILES_DIR}/axf2bin.cmake)
include(${c_CMAKE_FILES_DIR}/utility.cmake)
include(${c_CMAKE_FILES_DIR}/global_define.cmake)
import_kconfig("CONFIG" ${c_MCU_KCONFIG_FILE})
ameba_reset_global_define() #NOTE: Some variables like c_MP need to update after import kconfig
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
    ameba_execute_process(
        COMMAND ${CMAKE_OBJCOPY} -j .code -j .ARM.extab -j .ARM.exidx -j .ctors -j .dtors -j .preinit_array -j .init_array -j .fini_array -j .data -j .mmu_tbl -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/ram_2.bin
        COMMAND ${CMAKE_OBJCOPY} -j .xip_image2.text -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/xip_image2.bin
    )
else()
    ameba_execute_process(
        COMMAND ${CMAKE_OBJCOPY} -j .code -j .data -j .mmu_tbl -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/ram_2.bin
        COMMAND ${CMAKE_OBJCOPY} -j .xip_image2.text -j .ARM.extab -j .ARM.exidx -j .ctors -j .dtors -j .preinit_array -j .init_array -j .fini_array -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/xip_image2.bin
    )
endif()

if(CONFIG_BT)
    ameba_execute_process(
        COMMAND ${CMAKE_OBJCOPY} -j .bluetooth_trace.text -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/APP.trace
    )
endif()

ameba_execute_process(
    COMMAND ${CMAKE_COMMAND} -E cat ${c_SDK_IMAGE_TARGET_DIR}/ram_2.bin
    OUTPUT_FILE ${c_SDK_IMAGE_TARGET_DIR}/ca32_image2_all.bin
)

message( "========== Image manipulating start ==========")

# Call make -C atf image to generate fip.bin after ca32_image2_all.bin is generated
# TODO: need to catch the error
ameba_execute_process(
    COMMAND ${CMAKE_COMMAND} -E echo "Building atf img"
    COMMAND ${CMAKE_COMMAND} -E env make -j CROSS_COMPILE=${CROSS_COMPILE} PROJECT_DIR=${c_SOC_PROJECT_DIR}/project_ap BUILD_BASE=${BUILD_BASE} -C ${c_BASEDIR}/component/soc/${c_SOC_TYPE}/atf image
    WORKING_DIRECTORY ${c_BASEDIR}/component/soc/${c_SOC_TYPE}/atf
)

ameba_execute_process(
    COMMAND ${CMAKE_COMMAND} -E copy ${BUILD_BASE}/project_ap/asdk/make/atf/bl1/bl1.dump ${c_SDK_IMAGE_TARGET_DIR}/bl1.dump
    COMMAND ${CMAKE_COMMAND} -E copy ${BUILD_BASE}/project_ap/asdk/make/atf/bl2/bl2.dump ${c_SDK_IMAGE_TARGET_DIR}/bl2.dump
    COMMAND ${CMAKE_COMMAND} -E copy ${BUILD_BASE}/project_ap/asdk/make/atf/bl32/bl32.dump ${c_SDK_IMAGE_TARGET_DIR}/bl32.dump
    COMMAND ${CMAKE_COMMAND} -E copy ${BUILD_BASE}/project_ap/asdk/make/atf/bl1/bl1.map ${c_SDK_IMAGE_TARGET_DIR}/bl1.map
    COMMAND ${CMAKE_COMMAND} -E copy ${BUILD_BASE}/project_ap/asdk/make/atf/bl2/bl2.map ${c_SDK_IMAGE_TARGET_DIR}/bl2.map
    COMMAND ${CMAKE_COMMAND} -E copy ${BUILD_BASE}/project_ap/asdk/make/atf/bl32/bl32.map ${c_SDK_IMAGE_TARGET_DIR}/bl32.map
)

ameba_axf2bin_pad(${c_SDK_IMAGE_TARGET_DIR}/bl1_sram.bin 32)
ameba_axf2bin_pad(${c_SDK_IMAGE_TARGET_DIR}/bl1.bin 32)
ameba_axf2bin_pad(${c_SDK_IMAGE_TARGET_DIR}/fip.bin 32)
ameba_axf2bin_pad(${c_SDK_IMAGE_TARGET_DIR}/xip_image2.bin 32)
ameba_axf2bin_prepend_head(
    ${c_SDK_IMAGE_TARGET_DIR}/xip_image2_prepend.bin
    ${c_SDK_IMAGE_TARGET_DIR}/xip_image2.bin
    __flash_text_start__
    ${c_SDK_IMAGE_TARGET_DIR}/target_img2.map
)
ameba_axf2bin_prepend_head(
    ${c_SDK_IMAGE_TARGET_DIR}/bl1_sram_prepend.bin
    ${c_SDK_IMAGE_TARGET_DIR}/bl1_sram.bin
    __ca32_bl1_sram_start__
    ${c_SDK_IMAGE_TARGET_DIR}/target_img2.map
)
ameba_axf2bin_prepend_head(
    ${c_SDK_IMAGE_TARGET_DIR}/bl1_prepend.bin
    ${c_SDK_IMAGE_TARGET_DIR}/bl1.bin
    __ca32_bl1_dram_start__
    ${c_SDK_IMAGE_TARGET_DIR}/target_img2.map
)
ameba_axf2bin_prepend_head(
    ${c_SDK_IMAGE_TARGET_DIR}/fip_prepend.bin
    ${c_SDK_IMAGE_TARGET_DIR}/fip.bin
    __ca32_fip_dram_start__
    ${c_SDK_IMAGE_TARGET_DIR}/target_img2.map
)
ameba_execute_process(
    COMMAND ${CMAKE_COMMAND} -E cat ${c_SDK_IMAGE_TARGET_DIR}/xip_image2_prepend.bin ${c_SDK_IMAGE_TARGET_DIR}/bl1_sram_prepend.bin ${c_SDK_IMAGE_TARGET_DIR}/bl1_prepend.bin ${c_SDK_IMAGE_TARGET_DIR}/fip_prepend.bin
    OUTPUT_FILE ${c_SDK_IMAGE_TARGET_DIR}/ap_image_all.bin
)

if(CONFIG_DYNAMIC_APP_LOAD_EN)
    ameba_execute_process(
        COMMAND python ${c_BASEDIR}/component/dynamic_app_load/elf2tinf/imagetool.py imagetool ${c_BASEDIR}/component/dynamic_app_load/tinfbin/
    )

    if(EXISTS ${c_COMPONENT_DIR}/dynamic_app_load/tinfbin/dynamic_app.bin)
        ameba_axf2bin_prepend_head(
            ${c_COMPONENT_DIR}/dynamic_app_load/tinfbin/dynamic_app_prepend.bin
            ${c_COMPONENT_DIR}/dynamic_app_load/tinfbin/dynamic_app.bin
            __dram_dynamic_app_text_start__
            ${c_SDK_IMAGE_TARGET_DIR}/target_img2.map
        )

        ameba_execute_process(
            COMMAND ${CMAKE_COMMAND} -E rename ${c_SDK_IMAGE_TARGET_DIR}/ap_image_all.bin ${c_SDK_IMAGE_TARGET_DIR}/ap_image_all_tmp.bin
        )

        ameba_execute_process(
            COMMAND ${CMAKE_COMMAND} -E cat ${c_SDK_IMAGE_TARGET_DIR}/ap_image_all_tmp.bin ${c_BASEDIR}/component/dynamic_app_load/tinfbin/dynamic_app_prepend.bin
            OUTPUT_FILE ${c_SDK_IMAGE_TARGET_DIR}/ap_image_all.bin
        )

        ameba_execute_process(
            COMMAND ${CMAKE_COMMAND} -E remove ${c_SDK_IMAGE_TARGET_DIR}/ap_image_all_tmp.bin
        )

        if(EXISTS ${c_SDK_IMAGE_TARGET_DIR}/ap_image_all_en.bin)
            ameba_execute_process(
                COMMAND ${CMAKE_COMMAND} -E rename ${c_SDK_IMAGE_TARGET_DIR}/ap_image_all_en.bin ${c_SDK_IMAGE_TARGET_DIR}/ap_image_all_en_tmp.bin
            )

            ameba_execute_process(
                COMMAND ${CMAKE_COMMAND} -E cat ${c_SDK_IMAGE_TARGET_DIR}/ap_image_all_en_tmp.bin ${c_BASEDIR}/component/dynamic_app_load/tinfbin/dynamic_app_prepend.bin
                OUTPUT_FILE ${c_SDK_IMAGE_TARGET_DIR}/ap_image_all_en.bin
            )

            ameba_execute_process(
                COMMAND ${CMAKE_COMMAND} -E remove ${c_SDK_IMAGE_TARGET_DIR}/ap_image_all_en_tmp.bin
            )
        endif()
    endif()

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
