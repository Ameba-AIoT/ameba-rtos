set(CMAKE_EXECUTABLE_SUFFIX .axf)
set(CMAKE_VERBOSE_MAKEFILE OFF)


ameba_executable(target_img3_${PROJECT_NAME} ${c_EMPTY_C_FILE})
target_compile_definitions(target_img3_${PROJECT_NAME} PRIVATE IMAGE2_BUILD)

add_custom_target(
    image3_${PROJECT_NAME}
    COMMENT "build img3 only"
    DEPENDS target_img3_${PROJECT_NAME}
)

ameba_add_subdirectories_if(CONFIG_TRUSTZONE
    img3
    test
    fwlib
    os
    # mbedtls
)


get_property(IMG3_LIBS_PROPERTY GLOBAL PROPERTY g_${d_MCU_PROJECT_NAME}_IMAGE3_LIBS)

ameba_info("g_${d_MCU_PROJECT_NAME}_IMAGE3_LIBS: ${IMG3_LIBS_PROPERTY}")
ameba_info("d_LIST_${d_MCU_PROJECT_NAME_UPPER}_SECURE_LIBRARIES: ${d_LIST_${d_MCU_PROJECT_NAME_UPPER}_SECURE_LIBRARIES}")

target_link_libraries(
    target_img3_${PROJECT_NAME}
    PRIVATE
    -Wl,--out-implib=${CMAKE_CURRENT_BINARY_DIR}/cmse_implib.lib
    -Wl,--cmse-implib
    -T${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld
    d_PROJ_${d_MCU_PROJECT_NAME_UPPER}_LINK_OPTIONS
    -Wl,--gc-sections
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/text.map

    -Wl,--whole-archive
    ${IMG3_LIBS_PROPERTY}
    -Wl,--no-whole-archive
    ${d_LINK_ROM_LIB}

    ${d_LIST_${d_MCU_PROJECT_NAME_UPPER}_SECURE_LIBRARIES}
)

add_custom_command(
    OUTPUT ${IMAGE_TARGET_FOLDER}/target_img3.axf ${IMAGE_TARGET_FOLDER}/cmse_implib.lib
    COMMAND ${CMAKE_COMMAND} -E make_directory ${IMAGE_TARGET_FOLDER}
    COMMAND ${CMAKE_COMMAND} -E touch ${IMAGE_TARGET_FOLDER}/.gitignore
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/cmse_implib.lib ${IMAGE_TARGET_FOLDER}/cmse_implib.lib
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/text.map ${IMAGE_TARGET_FOLDER}/text_image3.map
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/target_img3_${PROJECT_NAME}.axf ${IMAGE_TARGET_FOLDER}/target_img3.axf
    COMMAND ${CMAKE_NM} ${IMAGE_TARGET_FOLDER}/target_img3.axf | sort > ${IMAGE_TARGET_FOLDER}/target_img3.map
    COMMAND ${CMAKE_OBJDUMP} -d ${IMAGE_TARGET_FOLDER}/target_img3.axf > ${IMAGE_TARGET_FOLDER}/target_img3.asm
    COMMAND ${CMAKE_COMMAND} -E copy ${IMAGE_TARGET_FOLDER}/target_img3.axf ${IMAGE_TARGET_FOLDER}/target_pure_img3.axf
    COMMAND ${CMAKE_STRIP} ${IMAGE_TARGET_FOLDER}/target_pure_img3.axf
    COMMAND ${CMAKE_NM} ${IMAGE_TARGET_FOLDER}/target_pure_img3.axf | sort > ${IMAGE_TARGET_FOLDER}/target_pure_img3.map

    ##secure
	COMMAND ${CMAKE_READELF} -s ${IMAGE_TARGET_FOLDER}/cmse_implib.lib > ${IMAGE_TARGET_FOLDER}/cmse_implib.txt
	COMMAND ${CMAKE_OBJDUMP} -j .gnu.sgstubs -d ${IMAGE_TARGET_FOLDER}/target_img3.axf > ${IMAGE_TARGET_FOLDER}/cmse_implib.asm

    COMMAND ${CMAKE_COMMAND} -E echo "  BIN      km4_image3_all.bin"

    COMMAND ${CMAKE_COMMAND}
    -Dd_PLATFORM_PROJECT_DIR=${d_PLATFORM_PROJECT_DIR}
    -Dd_MCU_PROJECT_DIR=${d_MCU_PROJECT_DIR}
    -DCONFIG_AMEBA_RLS=${CONFIG_AMEBA_RLS}
    -DIMAGE_TARGET_FOLDER=${IMAGE_TARGET_FOLDER} -DBASEDIR=${BASEDIR} -DKM4_PROJECT_DIR=${KM4_PROJECT_DIR} -DPROJECTDIR=${PROJECTDIR}
    -DIMAGESCRIPTDIR=${IMAGESCRIPTDIR} -DCMAKE_FILES_DIR=${CMAKE_FILES_DIR}
    -DCMAKE_SIZE=${CMAKE_SIZE} -DCMAKE_OBJCOPY=${CMAKE_OBJCOPY}
    -DBUILD_TYPE=${BUILD_TYPE} -DANALYZE_MP_IMG=${ANALYZE_MP_IMG} -DDAILY_BUILD=${DAILY_BUILD}
    -DCONFIG_BT=${CONFIG_BT} -DCONFIG_MP_SHRINK=${CONFIG_MP_SHRINK} -DCONFIG_MP_INCLUDED=${CONFIG_MP_INCLUDED}
    -DCONFIG_FATFS_WITHIN_APP_IMG=${CONFIG_FATFS_WITHIN_APP_IMG} -DCONFIG_COMPRESS_OTA_IMG=${CONFIG_COMPRESS_OTA_IMG} -DCONFIG_UPGRADE_BOOTLOADER=${CONFIG_UPGRADE_BOOTLOADER}
    -P ${CMAKE_CURRENT_SOURCE_DIR}/../img3_postbuild.cmake

    DEPENDS target_img3_${PROJECT_NAME}
    COMMENT "target_img3_${PROJECT_NAME} post build"
)

add_custom_target(
    cmse_implib_gen
    DEPENDS ${IMAGE_TARGET_FOLDER}/target_img3.axf ${IMAGE_TARGET_FOLDER}/cmse_implib.lib
)

add_custom_command(
  TARGET target_img3_${PROJECT_NAME} PRE_LINK
  COMMAND ${CMAKE_C_COMPILER} -E -P -xc -c ${d_SDK_LD_DIR}/ameba_img3_s.ld -o ${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld -I ${c_MENUCONFIG_DIR}
  COMMAND ${CMAKE_COMMAND} -E cat ${d_LINK_ROM_SYMBOL_S} >> ${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld
  COMMENT "build img3 linkerscript rlx8721d.ld"
)

set(
    LINK_SCRIPTS
    ${d_SDK_LD_DIR}/ameba_img3_s.ld
    ${d_PLATFORM_PROJECT_DIR}/amebasmart_layout.ld
)

set_target_properties(target_img3_${PROJECT_NAME} PROPERTIES LINK_DEPENDS "${LINK_SCRIPTS}")


set_property(TARGET target_img3_${PROJECT_NAME}
    PROPERTY ADDITIONAL_CLEAN_FILES
    ${IMAGE_TARGET_FOLDER}
)
