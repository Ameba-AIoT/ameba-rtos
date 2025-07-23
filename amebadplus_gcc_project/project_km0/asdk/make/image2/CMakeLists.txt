ameba_add_image(target_img2 p_IMAGE_ALL ${c_MCU_PROJECT_NAME}_image2_all.bin)
# variable c_CURRENT_IMAGE value is ${c_CURRENT_IMAGE}
###############################  ADD COMPONENT  ###################################

ameba_add_subdirectory_if_exist(${c_CMPT_APP_DIR}/benchmark)
ameba_add_subdirectory(${c_CMPT_AT_CMD_DIR})
ameba_add_subdirectory(${c_CMPT_BLUETOOTH_DIR})
ameba_add_subdirectory(${c_CMPT_BOOTLOADER_DIR})
ameba_add_subdirectory(${c_CMPT_CHIPINFO_DIR})
ameba_add_subdirectory(${c_CMPT_CMSIS_DIR})
ameba_add_subdirectory(${c_CMPT_CRASHDUMP_DIR})
ameba_add_subdirectory(${c_CMPT_FWLIB_DIR})

ameba_add_subdirectory(${c_CMPT_HAL_DIR})
ameba_add_subdirectory(${c_CMPT_MISC_DIR})
ameba_add_subdirectory(${c_CMPT_MONITOR_DIR})
ameba_add_subdirectory(${c_CMPT_OS_DIR})
ameba_add_subdirectory(${c_CMPT_PMC_DIR})
ameba_add_subdirectory(${c_CMPT_RTK_COEX_DIR})
ameba_add_subdirectory(${c_CMPT_LWIP_DIR})
ameba_add_subdirectory_if_exist(${c_CMPT_SOC_DIR}/verification)
ameba_add_subdirectory(${c_CMPT_SWLIB_DIR})
ameba_add_subdirectory(${c_CMPT_USRCFG_DIR})

ameba_add_subdirectory(${c_CMPT_WIFI_DIR})
ameba_add_subdirectory(${c_CMPT_WIFI_DIR}/wifi_fw)

ameba_add_subdirectory(${c_MCU_PROJECT_DIR}/src)


###############################  GENERATE IMAGE  ###################################
#---------------- PRE LINK -------------------#

add_custom_command(
  TARGET ${c_CURRENT_IMAGE} PRE_LINK
  COMMAND ${CMAKE_C_COMPILER} -E -P -xc -c ${c_IMG2_LD} -o ${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld -I ${c_MENUCONFIG_DIR}
  COMMAND ${CMAKE_COMMAND} -E cat ${c_LINK_ROM_SYMBOL} >> ${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld
  COMMENT "build img2 linkerscript rlx8721d.ld"
)
#----------------- LINKING -------------------#

target_link_libraries(${c_CURRENT_IMAGE}
    PRIVATE
    -T${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld
    ${c_MCU_PROJ_CONFIG}
    -Wl,--gc-sections
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/text.map

    -Wl,--whole-archive
    $<TARGET_PROPERTY:${c_MCU_PROJ_CONFIG},${c_CURRENT_IMAGE}_libraries>
    $<TARGET_PROPERTY:${c_MCU_PROJ_CONFIG},${c_CURRENT_IMAGE}_whole_archive_libs>
    -Wl,--no-whole-archive

    $<TARGET_PROPERTY:${c_MCU_PROJ_CONFIG},${c_CURRENT_IMAGE}_no_whole_archive_libs>

    ${c_LINK_ROM_LIB}
    -lm
)

#--------------- POST BUILD ------------------#
add_custom_command(
    OUTPUT ${c_SDK_IMAGE_TARGET_DIR}/target_img2.axf
    # TARGET ${c_CURRENT_IMAGE} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${c_SDK_IMAGE_TARGET_DIR}
    COMMAND ${CMAKE_COMMAND} -E touch ${c_SDK_IMAGE_TARGET_DIR}/.gitignore
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/text.map ${c_SDK_IMAGE_TARGET_DIR}/text_image2.map
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/${c_CURRENT_IMAGE}.axf ${c_SDK_IMAGE_TARGET_DIR}/target_img2.axf
    COMMAND ${CMAKE_NM} ${c_SDK_IMAGE_TARGET_DIR}/target_img2.axf | sort > ${c_SDK_IMAGE_TARGET_DIR}/target_img2.map
    COMMAND ${CMAKE_OBJDUMP} -d ${c_SDK_IMAGE_TARGET_DIR}/target_img2.axf > ${c_SDK_IMAGE_TARGET_DIR}/target_img2.asm
    COMMAND ${CMAKE_COMMAND} -E copy ${c_SDK_IMAGE_TARGET_DIR}/target_img2.asm ${KM4_PROJECT_DIR}/asdk/image/target_img2_otrcore.asm
    COMMAND ${CMAKE_COMMAND} -E copy ${c_SDK_IMAGE_TARGET_DIR}/target_img2.axf ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf
    COMMAND ${CMAKE_STRIP} ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf
    COMMAND ${CMAKE_NM} ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.axf | sort > ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img2.map

    COMMAND ${CMAKE_COMMAND} -E echo "  BIN      ${c_MCU_PROJECT_NAME}_image2_all.bin"

    COMMAND ${CMAKE_COMMAND}
        -Dc_BASEDIR=${c_BASEDIR}
        -Dc_CMAKE_FILES_DIR=${c_CMAKE_FILES_DIR}
        -Dc_SOC_PROJECT_DIR=${c_SOC_PROJECT_DIR}
        -Dc_MCU_PROJECT_DIR=${c_MCU_PROJECT_DIR}
        -Dc_MCU_PROJECT_NAME=${c_MCU_PROJECT_NAME} # name of current mcu project
        -Dc_MCU_KCONFIG_FILE=${c_MCU_KCONFIG_FILE}
        -Dc_SDK_IMAGE_TARGET_DIR=${c_SDK_IMAGE_TARGET_DIR}

        -DKM4_PROJECT_DIR=${KM4_PROJECT_DIR}
        -DFINAL_IMAGE_DIR=${FINAL_IMAGE_DIR}
        -DBUILD_TYPE=${BUILD_TYPE}
        -DANALYZE_MP_IMG=${ANALYZE_MP_IMG}
        -DDAILY_BUILD=${DAILY_BUILD}

        -DIMAGESCRIPTDIR=${IMAGESCRIPTDIR}
        -DCMAKE_SIZE=${CMAKE_SIZE}
        -DCMAKE_OBJCOPY=${CMAKE_OBJCOPY}

        -P ${CMAKE_CURRENT_SOURCE_DIR}/${c_POST_BUILD_SCRIPT}

    DEPENDS ${c_CURRENT_IMAGE}
)

add_custom_target(
    ${c_CURRENT_IMAGE}_postbuild
    ALL
    DEPENDS ${c_SDK_IMAGE_TARGET_DIR}/target_img2.axf
)

#------------- EXTRA PROCESS -----------------#
set(clean_files
    ${c_SDK_IMAGE_TARGET_DIR}
    ${FINAL_IMAGE_DIR}/km0_km4_app.bin
    ${FINAL_IMAGE_DIR}/ota_all.bin
    ${FINAL_IMAGE_DIR}/km0_km4_app_mp.bin
)

set(link_deps
    ${c_LAYOUT_LD}
    ${c_LINK_ROM_SYMBOL}
    ${c_IMG2_LD}
)

set_target_properties(${c_CURRENT_IMAGE} PROPERTIES
    #WARNING: values for these parameter MUST be placed inside a pair of quotes
    ADDITIONAL_CLEAN_FILES
        "${clean_files}"
    LINK_DEPENDS
        "${link_deps}"
)