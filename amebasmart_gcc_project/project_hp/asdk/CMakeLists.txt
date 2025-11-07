set(CMAKE_EXECUTABLE_SUFFIX .axf)

add_compile_options(-fdata-sections -ffunction-sections)

# note!!! Keep the latest version at the top for release reason.
if(CONFIG_AMEBASMART_B_CUT)
    ameba_set(c_ROM_LIB_DIR ${c_SDK_LIB_DIR}/amebasmart_rom_bcut)
    ameba_set(c_FLOADER_BIN_DIR ${c_SDK_GNU_UTILITY_DIR}/image_tool_flashloader/${c_SOC_TYPE}_acut)
else()
    ameba_fatal("CONFIG_AMEBASMART_B_CUT not defined as TRUE")
endif()

if(CONFIG_LINK_ROM_SYMB)
    ameba_unset(c_LINK_ROM_LIB)
    ameba_unset(c_LINK_ROM_LIB_NS)
    if(CONFIG_AMEBASMART_B_CUT)
        ameba_set(c_LINK_ROM_SYMBOL_NS    ${c_SDK_LD_DIR}/ameba_rom_symbol_acut.ld)
        ameba_set(c_LINK_ROM_SYMBOL_S     ${c_SDK_LD_DIR}/ameba_rom_symbol_bcut_s.ld)
    endif()
else()
    ameba_set(c_LINK_ROM_LIB ${c_ROM_LIB_DIR}/rom.a)
    ameba_set(c_LINK_ROM_LIB_NS ${c_ROM_LIB_DIR}/rom_ns.a)
endif()
ameba_list_append(c_LINK_ROM_SYMBOL_NS    ${c_SDK_LD_DIR}/ameba_rom_symbol_stdlib.ld)
ameba_add_subdirectory(make/image1)
ameba_add_subdirectory(make/image2)
ameba_add_subdirectory(make/image_floader)
ameba_add_subdirectory_ifnot(CONFIG_AMEBA_RLS make/image_imgtool_floader)

add_custom_target(
    ${c_MCU_PROJECT_NAME}
    COMMENT "build ${c_MCU_PROJECT_NAME} only"
    DEPENDS $<TARGET_PROPERTY:${c_MCU_PROJ_CONFIG},image_list>
)

add_custom_target(
    gdb
    COMMAND ${CMAKE_COMMAND} -E echo "Note: please use command :./build.py -gdb"
    COMMAND ${CMAKE_COMMAND} -E echo " -------------   HELP   -----------------"
    COMMAND ${CMAKE_COMMAND} -E echo " ./build.py -gdb for loading image with gdb"
    COMMAND ${CMAKE_COMMAND} -E echo " ./build.py -debug for debugging with gdb"
)

add_custom_target(
    gen_imgtool_floader
    COMMAND ${CMAKE_COMMAND} -DFILEPATH=${c_SDK_FLOADER_BIN_DIR}/floader_${c_SOC_TYPE}.bin -P${c_CMAKE_FILES_DIR}/checkexists.cmake
    COMMAND ${op_CUT} -i ${c_SDK_FLOADER_BIN_DIR}/floader_${c_SOC_TYPE}.bin -o ${c_SDK_IMAGE_TARGET_DIR}/ram_1_prepend.bin -l 4096 -B
    COMMAND ${op_FW_PACKAGE} --imgtool-floader ${c_SDK_IMAGE_TARGET_DIR}/ram_1_prepend.bin -o ${c_SDK_IMAGE_TARGET_DIR}/imgtool_flashloader.bin
    COMMAND ${CMAKE_COMMAND} -E copy ${c_SDK_IMAGE_TARGET_DIR}/imgtool_flashloader.bin ${c_SDK_FLOADER_BIN_DIR}/floader_${c_SOC_TYPE}.bin
)