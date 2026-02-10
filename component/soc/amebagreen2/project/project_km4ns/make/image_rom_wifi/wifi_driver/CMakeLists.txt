ameba_add_image(rom_wifi_chk p_EXCLUDE_FROM_ALL p_TYPE rom_ns)
# Use variable c_CURRENT_IMAGE to represent full image target name

###############################  ADD COMPONENT  ###################################

ameba_add_subdirectory_if_exist(${c_CMPT_WIFI_DIR}/rom)

###############################  GENERATE IMAGE  ###################################
#---------------- PRE LINK -------------------#
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/ameba_rom_wifi.ld ${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld
  COMMAND ${CMAKE_C_COMPILER} -E -P -xc -c ${c_SDK_LD_DIR}/ameba_rom_wifi.ld -o ${CMAKE_CURRENT_BINARY_DIR}/ameba_rom_wifi.ld -I ${c_MENUCONFIG_DIR}
  COMMAND ${CMAKE_C_COMPILER} -E -P -xc -c ${c_SDK_LD_DIR}/ameba_rom_bin_wifi.ld -o ${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld -I ${c_MENUCONFIG_DIR}
  COMMAND ${CMAKE_COMMAND} -E cat ${c_LINK_ROM_SYMBOL} >> ${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld
  COMMENT "build ld_rom"
  DEPENDS ${c_SDK_LD_DIR}/ameba_rom_wifi.ld ${c_SDK_LD_DIR}/ameba_rom_bin_wifi.ld ${c_LAYOUT_LD}
)

get_property(targets TARGET ${c_MCU_PROJ_CONFIG} PROPERTY ${c_CURRENT_IMAGE}_libraries)
set(target_libs)
foreach(t ${targets})
    ameba_list_append(target_libs $<TARGET_FILE:${t}>)
endforeach()

add_custom_command(
    OUTPUT ${c_ROM_LIB_DIR}/rom_wifi.a
    COMMAND ${CMAKE_LD}
        #  --verbose
        -r -d --warn-common
        -T ${CMAKE_CURRENT_BINARY_DIR}/ameba_rom_wifi.ld
        -Map=${CMAKE_CURRENT_BINARY_DIR}/rom_wifi.map
        --whole-archive
        ${target_libs}
        --no-whole-archive
        -o ${CMAKE_CURRENT_BINARY_DIR}/rom_wifi.o
    COMMAND ${CMAKE_AR} crv ${CMAKE_CURRENT_BINARY_DIR}/rom_wifi.a ${CMAKE_CURRENT_BINARY_DIR}/rom_wifi.o
    # COMMAND ${CMAKE_READELF} -S ${CMAKE_CURRENT_BINARY_DIR}/rom_wifi.o
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/rom_wifi.a ${c_ROM_LIB_DIR}/rom_wifi.a
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/rom_wifi.map ${c_ROM_LIB_DIR}/rom_wifi.map
    DEPENDS
        ${CMAKE_CURRENT_BINARY_DIR}/ameba_rom_wifi.ld
        $<TARGET_PROPERTY:${c_MCU_PROJ_CONFIG},${c_CURRENT_IMAGE}_libraries>
)

add_custom_target(
    gen_rom_wifi_a_${PROJECT_NAME}
    DEPENDS ${c_ROM_LIB_DIR}/rom_wifi.a
)

add_dependencies(${c_CURRENT_IMAGE} gen_rom_wifi_a_${PROJECT_NAME})

#----------------- LINKING -------------------#

target_link_libraries(${c_CURRENT_IMAGE}
    PRIVATE
    -nostdlib
    -T${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld
    ${c_MCU_PROJ_CONFIG}
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/text_rom_wifi.map

    -Wl,--whole-archive
    ${c_ROM_LIB_DIR}/rom_wifi.a
    -Wl,--no-whole-archive
)
target_link_options(${c_CURRENT_IMAGE} PRIVATE ${LINK_OPTIONS_ROM_WRAP})

#--------------- POST BUILD ------------------#
ameba_layout_extract(WIFI_ROM ${c_LAYOUT_LD} WIFI_ROM_ORIGIN WIFI_ROM_END WIFI_ROM_LENGTH)

add_custom_command(
    OUTPUT ${c_SDK_IMAGE_TARGET_DIR}/target_rom_wifi.axf ${c_SDK_LD_DIR}/ameba_rom_symbol_bcut_wifi.ld
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/text_rom_wifi.map ${c_SDK_IMAGE_TARGET_DIR}/text_rom_wifi.map
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/${c_CURRENT_IMAGE}.axf ${c_SDK_IMAGE_TARGET_DIR}/target_rom_wifi.axf
    COMMAND ${CMAKE_NM} ${c_SDK_IMAGE_TARGET_DIR}/target_rom_wifi.axf | sort > ${c_SDK_IMAGE_TARGET_DIR}/target_rom_wifi.map
    COMMAND ${CMAKE_OBJDUMP} -d ${c_SDK_IMAGE_TARGET_DIR}/target_rom_wifi.axf > ${c_SDK_IMAGE_TARGET_DIR}/target_rom_wifi.asm
    COMMAND ${CMAKE_COMMAND} -E copy ${c_SDK_IMAGE_TARGET_DIR}/target_rom_wifi.axf ${c_SDK_IMAGE_TARGET_DIR}/target_pure_rom_wifi.axf
    COMMAND ${CMAKE_STRIP} ${c_SDK_IMAGE_TARGET_DIR}/target_pure_rom_wifi.axf

    COMMAND ${CMAKE_OBJCOPY} -j .rom_wifi.text -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_rom_wifi.axf ${c_SDK_IMAGE_TARGET_DIR}/rom_wifi_chk.bin

    COMMAND ${op_PAD} -i ${c_SDK_IMAGE_TARGET_DIR}/rom_wifi_chk.bin -l ${WIFI_ROM_LENGTH}

    COMMAND python ${c_SDK_IMAGE_UTILITY_DIR}/rom_common_chk.py ${KM4TZ_BUILD_DIR}/${c_SDK_IMAGE_FOLDER_NAME}/rom_wifi.bin ${c_SDK_IMAGE_TARGET_DIR}/rom_wifi_chk.bin

    COMMAND ${CMAKE_COMMAND} -E echo "========== Image Info HEX NSEC =========="
    COMMAND ${CMAKE_SIZE} -A --radix=16 ${c_SDK_IMAGE_TARGET_DIR}/target_rom_wifi.axf
    COMMAND ${CMAKE_SIZE} -t --radix=16 ${c_SDK_IMAGE_TARGET_DIR}/target_rom_wifi.axf
    COMMAND ${CMAKE_COMMAND} -E echo "========== Image Info HEX NSEC =========="

    COMMAND ${CMAKE_COMMAND} -E echo "========== Image Info DEC NSEC =========="
    COMMAND ${CMAKE_SIZE} -A --radix=10 ${c_SDK_IMAGE_TARGET_DIR}/target_rom_wifi.axf
    COMMAND ${CMAKE_SIZE} -t --radix=10 ${c_SDK_IMAGE_TARGET_DIR}/target_rom_wifi.axf
    COMMAND ${CMAKE_COMMAND} -E echo "========== Image Info DEC NSEC =========="

    COMMAND ${CMAKE_COMMAND} -E copy
        ${c_SDK_IMAGE_TARGET_DIR}/text_rom_wifi.map
        ${c_SDK_IMAGE_TARGET_DIR}/target_rom_wifi.map
        ${c_SDK_IMAGE_TARGET_DIR}/target_rom_wifi.asm
        ${c_SDK_IMAGE_TARGET_DIR}/target_pure_rom_wifi.axf
        ${c_SDK_IMAGE_TARGET_DIR}/target_rom_wifi.axf
        ${c_SDK_IMAGE_TARGET_DIR}/rom_wifi_chk.bin
        ${c_ROM_LIB_DIR}

    COMMAND ${ROM_SYMBOL_GEN_PYTHON}
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/rom_wifi_symbol.txt ${c_SDK_LD_DIR}/ameba_rom_symbol_bcut_wifi.ld

    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${c_CURRENT_IMAGE}
)

add_custom_target(
    ${c_CURRENT_IMAGE}_postbuild
    DEPENDS ${c_SDK_IMAGE_TARGET_DIR}/target_rom_wifi.axf
)

