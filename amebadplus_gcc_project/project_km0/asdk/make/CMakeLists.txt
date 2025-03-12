ameba_executable(target_img2_${PROJECT_NAME} empty_file.c)

define_property(GLOBAL PROPERTY IMG2_LIBS_${PROJECT_NAME}
BRIEF_DOCS "internal library lists which will be linked to img2"
FULL_DOCS "internal library lists which will be linked to img2"
)
set_property(GLOBAL PROPERTY IMG2_LIBS_${PROJECT_NAME} "")

add_subdirectory(cmsis)
add_subdirectory(bootloader)
add_subdirectory(test)
add_subdirectory(os)
add_subdirectory(at_cmd)
add_subdirectory(target)
add_subdirectory(project)
add_subdirectory(utils)

if(CONFIG_APP_DEMO)
    add_subdirectory(example)
endif()

if(CONFIG_BT)
    add_subdirectory(bluetooth)
endif()

if(CONFIG_WLAN)
    if(CONFIG_AS_INIC_NP)
        add_subdirectory(${WIFIMAKEDIR} ${CMAKE_CURRENT_BINARY_DIR}/wlan)
    endif()
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
    ${LINK_ROM_LIB}
    ${LINK_THIRD_APP_LIB}
)

if(TARGET target_img2_km4_postbuild)
    #avoid making bin files in parallel
    set(DEP_KM4 target_img2_km4_postbuild)
endif()

add_custom_command(
    OUTPUT ${IMAGE_TARGET_FOLDER}/target_img2.axf
    # TARGET target_img2_${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${IMAGE_TARGET_FOLDER}
    COMMAND ${CMAKE_COMMAND} -E touch ${IMAGE_TARGET_FOLDER}/.gitignore
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/text.map ${IMAGE_TARGET_FOLDER}/text_image2.map
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/target_img2_${PROJECT_NAME}.axf ${IMAGE_TARGET_FOLDER}/target_img2.axf
    COMMAND ${CMAKE_NM} ${IMAGE_TARGET_FOLDER}/target_img2.axf | sort > ${IMAGE_TARGET_FOLDER}/target_img2.map
    COMMAND ${CMAKE_OBJDUMP} -d ${IMAGE_TARGET_FOLDER}/target_img2.axf > ${IMAGE_TARGET_FOLDER}/target_img2.asm
    COMMAND ${CMAKE_COMMAND} -E copy ${IMAGE_TARGET_FOLDER}/target_img2.asm ${KM4_BUILDDIR}/asdk/image/target_img2_otrcore.asm
    COMMAND ${CMAKE_COMMAND} -E copy ${IMAGE_TARGET_FOLDER}/target_img2.axf ${IMAGE_TARGET_FOLDER}/target_pure_img2.axf
    COMMAND ${CMAKE_STRIP} ${IMAGE_TARGET_FOLDER}/target_pure_img2.axf
    COMMAND ${CMAKE_NM} ${IMAGE_TARGET_FOLDER}/target_pure_img2.axf | sort > ${IMAGE_TARGET_FOLDER}/target_pure_img2.map

    COMMAND ${CMAKE_COMMAND} -E echo "  BIN      km0_image2_all.bin"

    COMMAND ${CMAKE_COMMAND}
    -DCONFIG_AMEBA_RLS=${CONFIG_AMEBA_RLS}
    -DIMAGE_TARGET_FOLDER=${IMAGE_TARGET_FOLDER} -DBASEDIR=${BASEDIR} -DKM4_BUILDDIR=${KM4_BUILDDIR} -DPROJECTDIR=${PROJECTDIR}
    -DIMAGESCRIPTDIR=${IMAGESCRIPTDIR} -DCMAKE_FILES_DIR=${CMAKE_FILES_DIR} -DFINAL_IMAGE_DIR=${FINAL_IMAGE_DIR}
    -DCMAKE_SIZE=${CMAKE_SIZE} -DCMAKE_OBJCOPY=${CMAKE_OBJCOPY}
    -DBUILD_TYPE=${BUILD_TYPE} -DANALYZE_MP_IMG=${ANALYZE_MP_IMG} -DDAILY_BUILD=${DAILY_BUILD}
    -DCONFIG_MP_SHRINK=${CONFIG_MP_SHRINK} -DCONFIG_MP_INCLUDED=${CONFIG_MP_INCLUDED}
    -DCONFIG_FATFS_WITHIN_APP_IMG=${CONFIG_FATFS_WITHIN_APP_IMG} -DCONFIG_COMPRESS_OTA_IMG=${CONFIG_COMPRESS_OTA_IMG} -DCONFIG_UPGRADE_BOOTLOADER=${CONFIG_UPGRADE_BOOTLOADER}
    -P ${CMAKE_CURRENT_SOURCE_DIR}/img2_postbuild.cmake

    DEPENDS target_img2_${PROJECT_NAME} ${DEP_KM4}
)

add_custom_target(
    target_img2_${PROJECT_NAME}_postbuild
    ALL
    DEPENDS ${IMAGE_TARGET_FOLDER}/target_img2.axf
)

if(CONFIG_MP_SHRINK)
    set(IMG2_LD ${LDDIR}/ameba_img2_all_mp.ld)
else()
    set(IMG2_LD ${LDDIR}/ameba_img2_all.ld)
endif()

add_custom_command(
  TARGET target_img2_${PROJECT_NAME} PRE_LINK
  COMMAND ${CMAKE_C_COMPILER} -E -P -xc -c ${IMG2_LD} -o ${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld -I ${MENUCONFIGDIR}
  COMMAND ${CMAKE_COMMAND} -E cat ${LINK_ROM_SYMBOL} >> ${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld
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
