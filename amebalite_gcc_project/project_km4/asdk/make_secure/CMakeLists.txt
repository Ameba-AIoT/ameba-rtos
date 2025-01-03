if(CONFIG_TRUSTZONE)
    ameba_executable(target_img3 "")

    add_dependencies(target_img2_${PROJECT_NAME} target_img3)

    add_custom_target(
        image3
        COMMENT "build img3 only"
        DEPENDS target_img3
    )

    target_link_libraries(
        target_img3
        PRIVATE ameba_interface_${PROJECT_NAME}
        -Wl,--out-implib=${CMAKE_CURRENT_BINARY_DIR}/cmse_implib.lib
        -Wl,--cmse-implib
        -Wl,--gc-sections
        -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/text.map
        -T${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld
        ${LINK_ROM_LIB}
    )

    set(IMG3_LD  ${LDDIR}/ameba_img3_all.ld)
    add_custom_command(
        TARGET target_img3 PRE_LINK
        COMMAND ${CMAKE_C_COMPILER} -E -P -xc -c ${IMG3_LD} -o ${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld
        COMMAND ${CMAKE_COMMAND} -E cat ${LINK_ROM_SYMBOL_S} >> ${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld
        COMMENT "build img3 linkerscript rlx8721d.ld"
    )

    set(
        LINK_SCRIPTS
        ${IMG3_LD}
        ${PROJECTDIR}/../amebalite_layout.ld
    )

    set_target_properties(target_img3 PROPERTIES LINK_DEPENDS "${LINK_SCRIPTS}")

    add_custom_command(
        OUTPUT ${IMAGE_TARGET_FOLDER}/cmse_implib.lib ${IMAGE_TARGET_FOLDER}/target_img3.axf
        COMMAND ${CMAKE_COMMAND} -E make_directory ${IMAGE_TARGET_FOLDER}
        COMMAND ${CMAKE_COMMAND} -E touch ${IMAGE_TARGET_FOLDER}/.gitignore
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/cmse_implib.lib ${IMAGE_TARGET_FOLDER}/cmse_implib.lib
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/text.map ${IMAGE_TARGET_FOLDER}/text_image3.map
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/target_img3.axf ${IMAGE_TARGET_FOLDER}
        COMMAND ${CMAKE_NM} ${IMAGE_TARGET_FOLDER}/target_img3.axf | sort > ${IMAGE_TARGET_FOLDER}/target_img3.map
        COMMAND ${CMAKE_OBJDUMP} -d ${IMAGE_TARGET_FOLDER}/target_img3.axf > ${IMAGE_TARGET_FOLDER}/target_img3.asm
        COMMAND ${CMAKE_COMMAND} -E copy ${IMAGE_TARGET_FOLDER}/target_img3.axf ${IMAGE_TARGET_FOLDER}/target_pure_img3.axf
        COMMAND ${CMAKE_STRIP}  ${IMAGE_TARGET_FOLDER}/target_pure_img3.axf
        COMMAND ${CMAKE_NM}  ${IMAGE_TARGET_FOLDER}/target_pure_img3.axf | sort > ${IMAGE_TARGET_FOLDER}/target_pure_img3.map

        COMMAND ${CMAKE_READELF} -s ${IMAGE_TARGET_FOLDER}/cmse_implib.lib > ${IMAGE_TARGET_FOLDER}/cmse_implib.txt
        COMMAND ${CMAKE_OBJDUMP} -j .gnu.sgstubs -d ${IMAGE_TARGET_FOLDER}/target_img3.axf > ${IMAGE_TARGET_FOLDER}/cmse_implib.asm

        COMMAND ${CMAKE_COMMAND} -E echo "  BIN      km4_image3_all.bin"
        COMMAND ${CMAKE_OBJCOPY}  -j .ram_image3.text -j .ram_image3.data -Obinary ${IMAGE_TARGET_FOLDER}/target_pure_img3.axf ${IMAGE_TARGET_FOLDER}/ram_3_s.bin
        COMMAND ${CMAKE_OBJCOPY}  -j .gnu.sgstubs -j .ram_image3.entry -Obinary ${IMAGE_TARGET_FOLDER}/target_pure_img3.axf ${IMAGE_TARGET_FOLDER}/ram_3_nsc.bin

        COMMAND ${CMAKE_COMMAND} -E echo "========== Image Info HEX =========="
        COMMAND ${CMAKE_SIZE} -A --radix=16 ${IMAGE_TARGET_FOLDER}/target_img3.axf
        COMMAND ${CMAKE_SIZE} -t --radix=16 ${IMAGE_TARGET_FOLDER}/target_img3.axf
        COMMAND ${CMAKE_COMMAND} -E echo "========== Image Info HEX =========="

        COMMAND ${CMAKE_COMMAND} -E echo "========== Image Info DEC =========="
        COMMAND ${CMAKE_SIZE} -A --radix=10 ${IMAGE_TARGET_FOLDER}/target_img3.axf
        COMMAND ${CMAKE_SIZE} -t --radix=10 ${IMAGE_TARGET_FOLDER}/target_img3.axf
        COMMAND ${CMAKE_COMMAND} -E echo "========== Image Info DEC =========="

        COMMAND ${CMAKE_COMMAND} -E echo "========== Image manipulating start =========="
        COMMAND ${PADTOOL} ${IMAGE_TARGET_FOLDER}/ram_3_s.bin 32
        COMMAND ${PADTOOL} ${IMAGE_TARGET_FOLDER}/ram_3_nsc.bin 32

        COMMAND ${PREPENDTOOL} ${IMAGE_TARGET_FOLDER}/ram_3_s.bin  __ram_image3_text_start__  ${IMAGE_TARGET_FOLDER}/target_img3.map
        COMMAND ${PREPENDTOOL} ${IMAGE_TARGET_FOLDER}/ram_3_nsc.bin  __ram_image3_nsc_start__  ${IMAGE_TARGET_FOLDER}/target_img3.map

        COMMAND ${CMAKE_COMMAND} -E cat ${IMAGE_TARGET_FOLDER}/ram_3_s_prepend.bin ${IMAGE_TARGET_FOLDER}/ram_3_nsc_prepend.bin > ${IMAGE_TARGET_FOLDER}/km4_image3_all.bin

        COMMAND ${IMAGETOOL} ${IMAGE_TARGET_FOLDER}/km4_image3_all.bin ${BUILD_TYPE}

        DEPENDS target_img3
        WORKING_DIRECTORY ${PROJECTDIR}/..

    )

    add_custom_target(
        target_img3_postbuild ALL
        DEPENDS ${IMAGE_TARGET_FOLDER}/cmse_implib.lib ${IMAGE_TARGET_FOLDER}/target_img3.axf
    )

    add_dependencies(target_img2_${PROJECT_NAME} target_img3_postbuild)

    add_subdirectory(img3)
    add_subdirectory(test)
    add_subdirectory(fwlib)
    add_subdirectory(os)
    # add_subdirectory(mbedtls)

    set_property(TARGET target_img3
    PROPERTY ADDITIONAL_CLEAN_FILES
    ${IMAGE_TARGET_FOLDER}
    )

endif()

