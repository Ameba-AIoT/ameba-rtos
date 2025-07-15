set(CMAKE_EXECUTABLE_SUFFIX .axf)

add_compile_options(-fdata-sections -ffunction-sections)

set(c_LAYOUT_LD ${c_SOC_PROJECT_DIR}/ameba_layout.ld)
if(CONFIG_MP_SHRINK)
    set(c_IMG2_LD ${c_SDK_LD_DIR}/ameba_img2_all_mp.ld)
else()
    set(c_IMG2_LD ${c_SDK_LD_DIR}/ameba_img2_all.ld)
endif()
set(c_IMG1_LD ${c_SDK_LD_DIR}/ameba_img1_all.ld)
set(c_IMG3_LD ${c_SDK_LD_DIR}/ameba_img3_all.ld)

if(CONFIG_AMEBADPLUS_A_CUT)
    set(c_ROM_LIB_DIR ${c_SDK_LIB_DIR}/amebadplus_rom_acut)
elseif(CONFIG_AMEBADPLUS_B_CUT)
    set(c_ROM_LIB_DIR ${c_SDK_LIB_DIR}/amebadplus_rom_bcut)
endif()

if(CONFIG_LINK_ROM_SYMB)
    set(c_LINK_ROM_LIB)
    set(c_LINK_ROM_LIB_NS)
    set(c_LINK_ROM_SYMBOL_NS ${c_SDK_LD_DIR}/ameba_rom_symbol_acut.ld)
    set(c_LINK_ROM_SYMBOL_S  ${c_SDK_LD_DIR}/ameba_rom_symbol_acut_s.ld)
else()
    set(c_LINK_ROM_LIB ${c_ROM_LIB_DIR}/rom.a)
    set(c_LINK_ROM_LIB_NS ${c_ROM_LIB_DIR}/rom_ns.a)
    set(c_LINK_ROM_SYMBOL_NS ${c_SDK_LD_DIR}/ameba_rom_symbol_empty.ld)
    set(c_LINK_ROM_SYMBOL_S ${c_SDK_LD_DIR}/ameba_rom_symbol_empty.ld)
endif()

if(CONFIG_TRUSTZONE)
    set(c_LINK_ROM_SYMBOL ${c_LINK_ROM_SYMBOL_NS})
else()
    set(c_LINK_ROM_SYMBOL ${c_LINK_ROM_SYMBOL_S})
endif()

list(APPEND c_LINK_ROM_SYMBOL ${c_SDK_LD_DIR}/ameba_rom_symbol_stdlib.ld)

ameba_add_subdirectory(make/image1)
ameba_add_subdirectory(make/image2)
get_property(image2_name TARGET ${c_MCU_PROJ_CONFIG} PROPERTY image2)
if(CONFIG_RDP_BASIC)
    ameba_add_subdirectory(make/image3)
    get_property(image3_name TARGET ${c_MCU_PROJ_CONFIG} PROPERTY image3)
    add_dependencies(${image2_name} ${image3_name}_postbuild)
elseif(CONFIG_RDP_TFM)
    ameba_add_subdirectory(make/tfm)
    add_dependencies(${image2_name} target_tfm)
endif()
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