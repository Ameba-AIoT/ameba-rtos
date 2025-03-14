# ameba_add_image(target_img2)
# variable c_CURRENT_IMAGE value is ${c_CURRENT_IMAGE}
set(c_CURRENT_IMAGE_TYPE "image2") #TODO: remove this line if ameba_add_image() above is uncommented
###############################  ADD COMPONENT  ###################################
# ameba_add_subdirectory(${c_CMPT_AT_CMD_DIR})
# ameba_add_subdirectory(${c_CMPT_AUDIO_DIR})
# ameba_add_subdirectory(${c_CMPT_BLUETOOTH_DIR})
# ameba_add_subdirectory(${c_CMPT_BOOTLOADER_DIR})
# ameba_add_subdirectory(${c_CMPT_CHIPINFO_DIR})
# ameba_add_subdirectory(${c_CMPT_CMSIS_DIR})
ameba_add_subdirectory(${c_CMPT_CRASHDUMP_DIR})
# ameba_add_subdirectory(${c_CMPT_FILE_SYSTEM_DIR})
ameba_add_subdirectory(${c_CMPT_FWLIB_DIR})
# ameba_add_subdirectory(${c_CMPT_HAL_DIR})
# ameba_add_subdirectory(${c_CMPT_MISC_DIR})
# ameba_add_subdirectory(${c_CMPT_MONITOR_DIR})
# ameba_add_subdirectory(${c_CMPT_NETWORK_DIR}) #port network/ and utilities
ameba_add_subdirectory(${c_CMPT_OS_DIR})
# ameba_add_subdirectory(${c_CMPT_PMC_DIR})
# ameba_add_subdirectory(${c_CMPT_RTK_COEX_DIR})
# ameba_add_subdirectory(${c_CMPT_SSL_DIR})
ameba_add_subdirectory(${c_CMPT_SWLIB_DIR}) #TODO: port libnosys/
# ameba_add_subdirectory(${c_CMPT_UI_DIR})
# ameba_add_subdirectory(${c_CMPT_UTILS_DIR})
ameba_add_subdirectory(${c_CMPT_USRCFG_DIR})
# ameba_add_subdirectory(${c_CMPT_WIFI_DIR}/wifi_make)
# ameba_add_subdirectory(${c_CMPT_WIFI_DIR}/wifi_fw)

return()
#unhandled add_subdirectory
add_subdirectory(project)
add_subdirectory(test)
add_subdirectory(application)

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

    ${SDK_TOOLCHAIN}/lib/gcc/riscv32-none-elf/${VSDK_VER}/rv32imafc/ilp32f/crtbegin.o
    ${SDK_TOOLCHAIN}/lib/gcc/riscv32-none-elf/${VSDK_VER}/rv32imafc/ilp32f/crtend.o
    -lm -lstdc++
)

#--------------- POST BUILD ------------------#

add_custom_command(
    OUTPUT ${IMAGE_TARGET_FOLDER}/target_img2.axf
    COMMAND ${CMAKE_COMMAND} -E make_directory ${IMAGE_TARGET_FOLDER}
    COMMAND ${CMAKE_COMMAND} -E touch ${IMAGE_TARGET_FOLDER}/.gitignore
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/text.map ${IMAGE_TARGET_FOLDER}/text_image2.map
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/target_img2_${PROJECT_NAME}.axf ${IMAGE_TARGET_FOLDER}/target_img2.axf
    COMMAND ${CMAKE_NM} ${IMAGE_TARGET_FOLDER}/target_img2.axf | sort > ${IMAGE_TARGET_FOLDER}/target_img2.map
    COMMAND ${CMAKE_OBJDUMP} --disassembler-options=no-aliases -d ${IMAGE_TARGET_FOLDER}/target_img2.axf > ${IMAGE_TARGET_FOLDER}/target_img2.asm
    COMMAND ${CMAKE_COMMAND} -E copy ${IMAGE_TARGET_FOLDER}/target_img2.axf ${IMAGE_TARGET_FOLDER}/target_pure_img2.axf
    COMMAND ${CMAKE_COMMAND} -E copy ${IMAGE_TARGET_FOLDER}/target_img2.asm ${KM4_PROJECT_DIR}/asdk/image/target_img2_otrcore.asm
    COMMAND ${CMAKE_STRIP} ${IMAGE_TARGET_FOLDER}/target_pure_img2.axf

    COMMAND ${CMAKE_COMMAND} -E echo "  BIN      kr4_image2_all.bin"

    COMMAND ${CMAKE_COMMAND}
        # common variables
        -Dc_BASEDIR=${c_BASEDIR}  # dir of sdk's root
        -Dc_CMAKE_FILES_DIR=${c_CMAKE_FILES_DIR} # dir of cmake scripts
        -Dc_SOC_PROJECT_DIR=${c_SOC_PROJECT_DIR} # dir of soc project
        -Dc_MCU_PROJECT_DIR=${c_MCU_PROJECT_DIR} # dir of current mcu project
        -Dc_MCU_KCONFIG_FILE=${c_MCU_KCONFIG_FILE} # file of current project's kconfig file
        -Dc_SDK_IMAGE_TARGET_DIR=${c_SDK_IMAGE_TARGET_DIR} # dir of image output

        # user's variables
        -DFINAL_IMAGE_DIR=${FINAL_IMAGE_DIR}
        -DBUILD_TYPE=${BUILD_TYPE}
        -DANALYZE_MP_IMG=${ANALYZE_MP_IMG}
        -DDAILY_BUILD=${DAILY_BUILD}

        # special variables for current image
        -DAP_IMAGEDIR=${AP_IMAGEDIR}

        # cmake toolchain variables
        -DIMAGESCRIPTDIR=${IMAGESCRIPTDIR}
        -DCMAKE_SIZE=${CMAKE_SIZE}
        -DCMAKE_OBJCOPY=${CMAKE_OBJCOPY}

        -P ${CMAKE_CURRENT_SOURCE_DIR}/postbuild.cmake
    DEPENDS target_img2_${PROJECT_NAME}
)


################################  EXTRA PROCESS  ###################################

set(clean_files
    ${c_SDK_IMAGE_TARGET_DIR}
    ${FINAL_IMAGE_DIR}/kr4_km4_app.bin
    ${FINAL_IMAGE_DIR}/kr4_km4_dsp_app.bin
    ${FINAL_IMAGE_DIR}/kr4_km4_app_mp.bin
    ${FINAL_IMAGE_DIR}/ota_all.bin
)

set(link_deps
    ${c_LAYOUT_LD}
    ${c_IMG2_LD}
    ${c_LINK_ROM_SYMBOL}
)

set_target_properties(${c_CURRENT_IMAGE} PROPERTIES
    #WARNING: values for these parameter MUST be placed inside a pair of quotes
    ADDITIONAL_CLEAN_FILES
        "${clean_files}"
    LINK_DEPENDS
        "${link_deps}"
)
