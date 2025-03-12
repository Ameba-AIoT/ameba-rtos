ameba_executable(target_img2_${PROJECT_NAME} empty_file.c)

add_custom_target(
    image2_${PROJECT_NAME}
    COMMENT "build img2 only"
    DEPENDS target_img2_${PROJECT_NAME}
)


define_property(GLOBAL PROPERTY IMG2_LIBS_${PROJECT_NAME}
BRIEF_DOCS "internal library lists which will be linked to img2"
FULL_DOCS "internal library lists which will be linked to img2"
)
set_property(GLOBAL PROPERTY IMG2_LIBS_${PROJECT_NAME} "")

add_subdirectory(at_cmd)
add_subdirectory(cmsis)
add_subdirectory(cmsis-dsp)
add_subdirectory(file_system)
add_subdirectory(libnosys)
add_subdirectory(os)
add_subdirectory(project)
add_subdirectory(target)
add_subdirectory(test)
add_subdirectory(utils)

if(CONFIG_AUDIO_FWK)
    add_subdirectory(audio/audio_config)
    add_subdirectory(audio/audio_hal)
    # add_subdirectory(audio/audio_test)
    # add_subdirectory(audio/audio_hal/test)
    # add_subdirectory(audio/base/cutils/cmd)
endif()

if(CONFIG_TFLITE_MICRO_EN)
    add_subdirectory(tflite_micro)
endif()

if(CONFIG_BT)
    add_subdirectory(bluetooth)
endif()

if(CONFIG_802154_EN)
    add_subdirectory(wpan)
endif()

if(CONFIG_WLAN)
    add_subdirectory(${WIFIMAKEDIR} ${CMAKE_CURRENT_BINARY_DIR}/wlan)
    if(CONFIG_AS_INIC_AP)
        add_subdirectory(application)
        if(NOT CONFIG_MP_SHRINK)
            add_subdirectory(network)
        endif()
        add_subdirectory(utilities)
        add_subdirectory(utilities_example)
    elseif(CONFIG_AS_INIC_NP)
        if(CONFIG_WPA_STD)
            add_subdirectory(network)
        endif()
    elseif(CONFIG_SINGLE_CORE_WIFI)
        add_subdirectory(application)
        if(NOT CONFIG_MP_SHRINK)
            add_subdirectory(network)
        endif()
        add_subdirectory(utilities)
        add_subdirectory(utilities_example)
    endif()
endif()

if(CONFIG_USB_DEVICE_EN)
    add_subdirectory(usb_otg)
endif()

if(CONFIG_MBEDTLS_ENABLED)
    add_subdirectory(mbedtls)
endif()

if(CONFIG_GUI_EN)
    add_subdirectory(ui)
endif()

if(CONFIG_LINKKIT_AWSS)
    add_subdirectory(linkkit)
endif()

get_property(IMG2_LIBS_PROPERTY GLOBAL PROPERTY IMG2_LIBS_${PROJECT_NAME})

target_link_libraries(
    target_img2_${PROJECT_NAME}
    PRIVATE ameba_interface_${PROJECT_NAME} ameba_wrap_interface_${PROJECT_NAME}
    -Wl,--gc-sections
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/text.map
    -T${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld
    -Wl,--whole-archive
    ${IMG2_LIBS_PROPERTY}
    ${LINK_APP_LIB}
    -Wl,--no-whole-archive
    ${LINK_ROM_LIB_NS}
    ${LINK_THIRD_APP_LIB}
    ${SDK_TOOLCHAIN}/lib/gcc/arm-none-eabi/${ASDK_VER}/thumb/v8-m.main+fp/fpu/crtbegin.o
    ${SDK_TOOLCHAIN}/lib/gcc/arm-none-eabi/${ASDK_VER}/thumb/v8-m.main+fp/fpu/crtend.o
    -lm -lstdc++
)


add_custom_command(
    OUTPUT ${IMAGE_TARGET_FOLDER}/target_img2.axf ${KM0_BUILDDIR}/asdk/image/target_img2_otrcore.asm
    COMMAND ${CMAKE_COMMAND} -E make_directory ${IMAGE_TARGET_FOLDER}
    COMMAND ${CMAKE_COMMAND} -E touch ${IMAGE_TARGET_FOLDER}/.gitignore
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/text.map ${IMAGE_TARGET_FOLDER}/text_image2_ns.map
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/target_img2_${PROJECT_NAME}.axf ${IMAGE_TARGET_FOLDER}/target_img2.axf
    COMMAND ${CMAKE_NM} ${IMAGE_TARGET_FOLDER}/target_img2.axf | sort > ${IMAGE_TARGET_FOLDER}/target_img2.map
    COMMAND ${CMAKE_OBJDUMP} -d ${IMAGE_TARGET_FOLDER}/target_img2.axf > ${IMAGE_TARGET_FOLDER}/target_img2.asm
    COMMAND ${CMAKE_COMMAND} -E copy ${IMAGE_TARGET_FOLDER}/target_img2.axf ${IMAGE_TARGET_FOLDER}/target_pure_img2.axf
    COMMAND ${CMAKE_COMMAND} -E copy ${IMAGE_TARGET_FOLDER}/target_img2.asm ${KM0_BUILDDIR}/asdk/image/target_img2_otrcore.asm
    COMMAND ${CMAKE_STRIP} ${IMAGE_TARGET_FOLDER}/target_pure_img2.axf

    COMMAND ${CMAKE_COMMAND} -E echo "  BIN      km4_image2_all.bin"

    COMMAND ${CMAKE_COMMAND}
    -DCONFIG_AMEBA_RLS=${CONFIG_AMEBA_RLS}
    -DIMAGE_TARGET_FOLDER=${IMAGE_TARGET_FOLDER} -DBASEDIR=${BASEDIR} -DPROJECTDIR=${PROJECTDIR}
    -DIMAGESCRIPTDIR=${IMAGESCRIPTDIR} -DCMAKE_FILES_DIR=${CMAKE_FILES_DIR} -DFINAL_IMAGE_DIR=${FINAL_IMAGE_DIR}
    -DCMAKE_SIZE=${CMAKE_SIZE} -DCMAKE_OBJCOPY=${CMAKE_OBJCOPY}
    -DBUILD_TYPE=${BUILD_TYPE} -DANALYZE_MP_IMG=${ANALYZE_MP_IMG} -DDAILY_BUILD=${DAILY_BUILD}
    -DCONFIG_BT=${CONFIG_BT} -DCONFIG_MP_SHRINK=${CONFIG_MP_SHRINK} -DCONFIG_MP_INCLUDED=${CONFIG_MP_INCLUDED}
    -DCONFIG_FATFS_WITHIN_APP_IMG=${CONFIG_FATFS_WITHIN_APP_IMG} -DCONFIG_COMPRESS_OTA_IMG=${CONFIG_COMPRESS_OTA_IMG} -DCONFIG_UPGRADE_BOOTLOADER=${CONFIG_UPGRADE_BOOTLOADER}
    -P ${CMAKE_CURRENT_SOURCE_DIR}/img2_postbuild.cmake

    DEPENDS target_img2_${PROJECT_NAME}
)

add_custom_target(
    target_img2_${PROJECT_NAME}_postbuild
    ALL
    DEPENDS target_wifi_nouse
)

add_custom_target(
    target_wifi_nouse
    DEPENDS ${IMAGE_TARGET_FOLDER}/target_img2.axf ${KM0_BUILDDIR}/asdk/image/target_img2_otrcore.asm
)

if(CONFIG_MP_SHRINK)
    set(IMG2_LD ${LDDIR}/ameba_img2_all_mp.ld)
else()
    set(IMG2_LD ${LDDIR}/ameba_img2_all.ld)
endif()

if(CONFIG_TRUSTZONE)
    set(LINK_ROM_SYMBOL ${LINK_ROM_SYMBOL_NS})
else()
    set(LINK_ROM_SYMBOL ${LINK_ROM_SYMBOL_S})
endif()


add_custom_command(
  TARGET target_img2_${PROJECT_NAME} PRE_LINK
  COMMAND ${CMAKE_C_COMPILER} -E -P -xc -c ${IMG2_LD} -o ${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld -I ${MENUCONFIGDIR}
  COMMAND ${CMAKE_COMMAND} -E cat ${LINK_ROM_SYMBOL} >> ${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld
  COMMAND ${CMAKE_COMMAND} -E cat ${LINK_ROM_SYMBOL_STDLIB} >> ${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld
  COMMENT "build img2 linkerscript rlx8721d.ld"
)

set(
    LINK_SCRIPTS
    ${IMG2_LD}
    ${PROJECTDIR}/../amebaDplus_layout.ld
)

set_target_properties(target_img2_${PROJECT_NAME} PROPERTIES LINK_DEPENDS "${LINK_SCRIPTS}")


set_property(TARGET target_img2_${PROJECT_NAME}
    PROPERTY ADDITIONAL_CLEAN_FILES
    ${IMAGE_TARGET_FOLDER}
    ${FINAL_IMAGE_DIR}/km0_km4_app.bin
    ${FINAL_IMAGE_DIR}/km0_km4_app_mp.bin
    ${FINAL_IMAGE_DIR}/ota_all.bin
)
