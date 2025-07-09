set(CMAKE_EXECUTABLE_SUFFIX .axf)

set(c_LAYOUT_LD ${c_SOC_PROJECT_DIR}/ameba_layout.ld)
set(c_IMG1_LD ${c_SDK_LD_DIR}/ameba_img1_all.ld)
set(c_IMG2_LD ${c_SDK_LD_DIR}/ameba_img2_all.ld)
set(c_IMG3_LD ${c_SDK_LD_DIR}/ameba_img3_all.ld)
if(CONFIG_AMEBAL2_A_CUT)
    set(c_ROM_LIB_DIR  ${c_SDK_LIB_DIR}/amebaL2_rom_acut)
endif()

if(CONFIG_LINK_ROM_SYMB)
    set(c_LINK_ROM_LIB)
    set(c_LINK_ROM_LIB_NS)
    set(c_LINK_ROM_SYMBOL_S  ${c_SDK_LD_DIR}/ameba_rom_symbol_acut_s.ld)
    set(c_LINK_ROM_SYMBOL_NS ${c_SDK_LD_DIR}/ameba_rom_symbol_acut.ld)
    set(c_LINK_ROM_SYMBOL_OS ${c_SDK_LD_DIR}/ameba_rom_symbol_acut_os.ld)
    set(c_LINK_ROM_SYMBOL_WIFI ${c_SDK_LD_DIR}/ameba_rom_symbol_acut_wifi.ld)
else()
    set(c_LINK_ROM_LIB ${c_ROM_LIB_DIR}/rom.a)
    set(c_LINK_ROM_SYMBOL_S ${c_SDK_LD_DIR}/ameba_rom_symbol_empty.ld)
    set(c_LINK_ROM_SYMBOL_NS ${c_SDK_LD_DIR}/ameba_rom_symbol_empty.ld)
endif()
#if not config trustzone, link secure rom symbol. TODO
# if(CONFIG_TRUSTZONE)
    set(c_LINK_ROM_SYMBOL ${c_LINK_ROM_SYMBOL_NS})
# else()
#     set(c_LINK_ROM_SYMBOL ${c_LINK_ROM_SYMBOL_S})
# endif()


# ameba_add_subdirectory_if_exist(make/image_rom)

#NOTE: These compile options are not required for rom's component
add_compile_options(-fdata-sections -ffunction-sections)

#WARNING: image_gdb_floader MUST be added before image1
ameba_add_subdirectory_if(CONFIG_FPGA make/image_gdb_floader)
ameba_add_subdirectory_ifnot(CONFIG_AMEBA_RLS make/image_imgtool_floader)

ameba_add_subdirectory_if(CONFIG_WIFI_HOST_CONTROL make/image_pg_loader)

ameba_add_subdirectory(make/image1)
ameba_add_subdirectory(make/image2)

if(CONFIG_TRUSTZONE)
    ameba_add_subdirectory(make/image3)
    ameba_get_image_target_name(image2 image2_name)
    ameba_get_image_target_name(image3 image3_name)
    add_dependencies(${image2_name} ${image3_name}_postbuild)
endif()

add_custom_target(
    gdb
    COMMAND ${CMAKE_COMMAND} -E echo "Note: please use command :./build.py -gdb"
    COMMAND ${CMAKE_COMMAND} -E echo " -------------   HELP   -----------------"
    COMMAND ${CMAKE_COMMAND} -E echo " ./build.py -gdb for loading image with gdb"
    COMMAND ${CMAKE_COMMAND} -E echo " ./build.py -debug for debugging with gdb"
)

add_custom_target(
    gen_imgtool_floader
    COMMAND ${CMAKE_COMMAND} -DFILEPATH=${c_SDK_FLOADER_BIN_DIR}/imgtool_flashloader_raw.bin -P ${c_CMAKE_FILES_DIR}/checkexists.cmake
    COMMAND ${CMAKE_COMMAND} -E copy ${c_SDK_FLOADER_BIN_DIR}/imgtool_flashloader_raw.bin ${c_SDK_IMAGE_TARGET_DIR}/ram_1_prepend.bin
    COMMAND ${IMAGETOOL} ${c_SDK_IMAGE_TARGET_DIR}/ram_1_prepend.bin ${BUILD_TYPE}
    COMMAND ${CMAKE_COMMAND} -E copy ${c_SDK_IMAGE_TARGET_DIR}/imgtool_flashloader.bin ${c_SDK_FLOADER_BIN_DIR}/floader_amebagreen2.bin
)
