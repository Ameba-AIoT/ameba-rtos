ameba_add_image(target_img2_${PROJECT_NAME})
# variable c_CURRENT_IMAGE value is target_img2_${PROJECT_NAME}

###############################  ADD COMPONENT  ###################################

ameba_add_subdirectory(${c_CMPT_AT_CMD_DIR})
ameba_add_subdirectory(${c_CMPT_FWLIB_DIR})
ameba_add_subdirectory(${c_CMPT_HAL_DIR})
ameba_add_subdirectory(${c_CMPT_MISC_DIR})
ameba_add_subdirectory(${c_CMPT_AUDIO_DIR})
ameba_add_subdirectory(${c_CMPT_EXAMPLE_DIR})
ameba_add_subdirectory(${c_CMPT_FILE_SYSTEM_DIR})
ameba_add_subdirectory(${c_CMPT_APP_DIR})
ameba_add_subdirectory(${c_CMPT_OS_DIR})
ameba_add_subdirectory(${c_CMPT_UTILS_DIR})
ameba_add_subdirectory(${c_CMPT_BLUETOOTH_DIR})
ameba_add_subdirectory(${c_CMPT_NETWORK_DIR})
ameba_add_subdirectory(${c_CMPT_SSL_DIR})
ameba_add_subdirectory(${c_CMPT_UI_DIR})
ameba_add_subdirectory(${c_CMPT_RTK_COEX_DIR})

ameba_add_subdirectory(${c_CMPT_WIFI_DIR}/wifi_make)
ameba_add_subdirectory(${c_CMPT_USB_DIR})

ameba_add_subdirectory(${c_CMPT_CMSIS_DSP_DIR})
ameba_add_subdirectory(${c_CMPT_SOC_DIR}/verification)
ameba_add_subdirectory(${c_CMPT_CRASHDUMP_DIR})

ameba_add_subdirectory(${c_CMPT_CHIPINFO_DIR})
ameba_add_subdirectory(${c_CMPT_PMC_DIR})

ameba_add_subdirectory(${c_MCU_PROJECT_DIR}/src)

#unhandled component
ameba_add_subdirectories(
    make/project
)

###############################  GENERATE IMAGE  ###################################
#---------------- PRE LINK -------------------#
if(CONFIG_XIP_FLASH)
    set(IMG2_LD ${c_SDK_LD_DIR}/ameba_img2_non_trustzone_xip.ld)
else()
    set(IMG2_LD ${c_SDK_LD_DIR}/ameba_img2_non_trustzone_dram.ld)
endif()

add_custom_command(
  TARGET ${c_CURRENT_IMAGE} PRE_LINK
  COMMAND ${CMAKE_C_COMPILER} -E -P -xc -c ${IMG2_LD} -o ${CMAKE_CURRENT_BINARY_DIR}/make/rlx8721d.ld
  COMMAND ${CMAKE_COMMAND} -E cat ${c_LINK_ROM_SYMBOL_NS} >> ${CMAKE_CURRENT_BINARY_DIR}/make/rlx8721d.ld
  COMMENT "build img2 linkerscript rlx8721d.ld"
)

#----------------- LINKING -------------------#
target_link_libraries(${c_CURRENT_IMAGE}
    PRIVATE
    -T${CMAKE_CURRENT_BINARY_DIR}/make/rlx8721d.ld
    ${c_MCU_PROJ_CONFIG}
    -Wl,--gc-sections
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/text.map

    -Wl,--whole-archive
    $<TARGET_PROPERTY:${c_MCU_PROJ_CONFIG},${c_CURRENT_IMAGE}_libraries>
    $<TARGET_PROPERTY:${c_MCU_PROJ_CONFIG},${c_CURRENT_IMAGE}_whole_archive_libs>
    -Wl,--no-whole-archive

    $<TARGET_PROPERTY:${c_MCU_PROJ_CONFIG},${c_CURRENT_IMAGE}_no_whole_archive_libs>
    ${c_LINK_ROM_LIB}

    ${c_TOOLCHAIN_DIR}/lib/gcc/arm-none-eabi/${c_SDK_VERSION}/thumb/v8-m.main+fp/fpu/crtbegin.o
    ${c_TOOLCHAIN_DIR}/lib/gcc/arm-none-eabi/${c_SDK_VERSION}/thumb/v8-m.main+fp/fpu/crtend.o
    -lm -lstdc++
)

#--------------- POST BUILD ------------------#

add_custom_command(
    TARGET ${c_CURRENT_IMAGE} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${c_SDK_IMAGE_TARGET_FOLDER}
    COMMAND ${CMAKE_COMMAND} -E touch ${c_SDK_IMAGE_TARGET_FOLDER}/.gitignore
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/text.map ${c_SDK_IMAGE_TARGET_FOLDER}/text_image2_ns.map
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/${c_CURRENT_IMAGE}.axf ${c_SDK_IMAGE_TARGET_FOLDER}/target_img2.axf
    COMMAND ${CMAKE_NM} ${c_SDK_IMAGE_TARGET_FOLDER}/target_img2.axf | sort > ${c_SDK_IMAGE_TARGET_FOLDER}/target_img2.map
    COMMAND ${CMAKE_OBJDUMP} -d ${c_SDK_IMAGE_TARGET_FOLDER}/target_img2.axf > ${c_SDK_IMAGE_TARGET_FOLDER}/target_img2.asm
    COMMAND ${CMAKE_COMMAND} -E copy ${c_SDK_IMAGE_TARGET_FOLDER}/target_img2.axf ${c_SDK_IMAGE_TARGET_FOLDER}/target_pure_img2.axf
    COMMAND ${CMAKE_COMMAND} -E copy ${c_SDK_IMAGE_TARGET_FOLDER}/target_img2.asm ${AP_PROJECT_DIR}/asdk/image/target_img2_otrcore.asm
    COMMAND ${CMAKE_STRIP} ${c_SDK_IMAGE_TARGET_FOLDER}/target_pure_img2.axf

    COMMAND ${CMAKE_COMMAND} -E echo "  BIN      km4_image2_all.bin"

    COMMAND ${CMAKE_COMMAND}
    -Dc_SOC_PROJECT_DIR=${c_SOC_PROJECT_DIR}
    -Dd_MCU_PROJECT_DIR=${c_MCU_PROJECT_DIR}
    -DFINAL_IMAGE_DIR=${FINAL_IMAGE_DIR}
    -DCONFIG_AMEBA_RLS=${CONFIG_AMEBA_RLS}
    -DCONFIG_LINUX_FW_EN=${CONFIG_LINUX_FW_EN}
    -Dc_SDK_IMAGE_TARGET_FOLDER=${c_SDK_IMAGE_TARGET_FOLDER} -Dc_BASEDIR=${c_BASEDIR} -DKM4_PROJECT_DIR=${KM4_PROJECT_DIR} -Dc_MCU_PROJECT_DIR=${c_MCU_PROJECT_DIR}
    -DIMAGESCRIPTDIR=${IMAGESCRIPTDIR} -Dc_CMAKE_FILES_DIR=${c_CMAKE_FILES_DIR}
    -DCMAKE_SIZE=${CMAKE_SIZE} -DCMAKE_OBJCOPY=${CMAKE_OBJCOPY}
    -DBUILD_TYPE=${BUILD_TYPE} -DANALYZE_MP_IMG=${ANALYZE_MP_IMG} -DDAILY_BUILD=${DAILY_BUILD}
    -DCONFIG_BT=${CONFIG_BT} -DCONFIG_MP_SHRINK=${CONFIG_MP_SHRINK} -DCONFIG_MP_INCLUDED=${CONFIG_MP_INCLUDED}
    -DCONFIG_FATFS_WITHIN_APP_IMG=${CONFIG_FATFS_WITHIN_APP_IMG} -DCONFIG_COMPRESS_OTA_IMG=${CONFIG_COMPRESS_OTA_IMG} -DCONFIG_UPGRADE_BOOTLOADER=${CONFIG_UPGRADE_BOOTLOADER}
    -P ${CMAKE_CURRENT_SOURCE_DIR}/img2_postbuild.cmake

    DEPENDS ${c_CURRENT_IMAGE}
    COMMENT "${c_CURRENT_IMAGE} post build"
)

#------------- EXTRA PROCESS -----------------#
set(clean_files
    ${c_SDK_IMAGE_TARGET_FOLDER}
    ${FINAL_IMAGE_DIR}/km0_km4_app.bin
    ${FINAL_IMAGE_DIR}/km0_km4_ca32_app.bin
    ${FINAL_IMAGE_DIR}/ota_all.bin
    ${FINAL_IMAGE_DIR}/km4_boot_all.bin
    ${FINAL_IMAGE_DIR}/km0_km4_app_mp.bin
    ${FINAL_IMAGE_DIR}/km0_km4_ca32_app_mp.bin
)

set(link_deps
    ${IMG2_LD}
    ${c_SOC_PROJECT_DIR}/${c_SOC_TYPE}_layout.ld
)

set_target_properties(${c_CURRENT_IMAGE} PROPERTIES
    #WARNING: values for these parameter MUST be placed inside a pair of quotes
    ADDITIONAL_CLEAN_FILES
        "${clean_files}"
    LINK_DEPENDS
        "${link_deps}"
)