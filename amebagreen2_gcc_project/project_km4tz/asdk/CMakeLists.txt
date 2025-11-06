set(CMAKE_EXECUTABLE_SUFFIX .axf)

set(c_LAYOUT_LD ${c_SOC_PROJECT_DIR}/ameba_layout.ld)
set(c_IMG1_LD ${c_SDK_LD_DIR}/ameba_img1_all.ld)
set(c_IMG2_LD ${c_SDK_LD_DIR}/ameba_img2_all.ld)
set(c_IMG3_LD ${c_SDK_LD_DIR}/ameba_img3_all.ld)
if(CONFIG_AMEBAGREEN2_A_CUT)
    set(c_ROM_LIB_DIR  ${c_SDK_LIB_DIR}/amebagreen2_rom_acut)
elseif(CONFIG_AMEBAGREEN2_B_CUT)
    set(c_ROM_LIB_DIR  ${c_SDK_LIB_DIR}/amebagreen2_rom_bcut)
endif()

if(CONFIG_LINK_ROM_SYMB)
    set(c_LINK_ROM_LIB)
    set(c_LINK_ROM_LIB_NS)
    if(CONFIG_AMEBAGREEN2_A_CUT)
        set(c_LINK_ROM_SYMBOL_S  ${c_SDK_LD_DIR}/ameba_rom_symbol_acut_s.ld)
        set(c_LINK_ROM_SYMBOL_NS ${c_SDK_LD_DIR}/ameba_rom_symbol_acut.ld)
        set(c_LINK_ROM_SYMBOL_OS ${c_SDK_LD_DIR}/ameba_rom_symbol_acut_os.ld)
        set(c_LINK_ROM_SYMBOL_WIFI ${c_SDK_LD_DIR}/ameba_rom_symbol_acut_wifi.ld)
    elseif(CONFIG_AMEBAGREEN2_B_CUT)
        set(c_LINK_ROM_SYMBOL_S  ${c_SDK_LD_DIR}/ameba_rom_symbol_bcut_s.ld)
        set(c_LINK_ROM_SYMBOL_NS ${c_SDK_LD_DIR}/ameba_rom_symbol_bcut.ld)
        set(c_LINK_ROM_SYMBOL_OS ${c_SDK_LD_DIR}/ameba_rom_symbol_bcut_os.ld)
        set(c_LINK_ROM_SYMBOL_WIFI ${c_SDK_LD_DIR}/ameba_rom_symbol_bcut_wifi.ld)
    endif()
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

if(CONFIG_FPGA)
    ameba_add_subdirectory_if_exist(make/image_rom_wifi)
endif()

#NOTE: These compile options are not required for rom's component
add_compile_options(-fdata-sections -ffunction-sections)

#WARNING: image_gdb_floader MUST be added before image1
ameba_add_subdirectory_if(CONFIG_FPGA make/image_gdb_floader)
ameba_add_subdirectory_ifnot(CONFIG_AMEBA_RLS make/image_imgtool_floader)

if ((NOT CONFIG_WHC_INTF_IPC) AND CONFIG_WHC_DEV)
ameba_add_subdirectory(make/image_pg_loader)
endif()

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
    COMMAND ${CMAKE_COMMAND} -DFILEPATH=${c_SDK_FLOADER_BIN_DIR}/floader_${c_SOC_TYPE}.bin -P${c_CMAKE_FILES_DIR}/checkexists.cmake
    COMMAND ${op_CUT} -i ${c_SDK_FLOADER_BIN_DIR}/floader_${c_SOC_TYPE}.bin -o ${c_SDK_IMAGE_TARGET_DIR}/ram_1_prepend.bin -l 4096 -B
    COMMAND ${op_FW_PACKAGE} --imgtool-floader ${c_SDK_IMAGE_TARGET_DIR}/ram_1_prepend.bin -o ${c_SDK_IMAGE_TARGET_DIR}/imgtool_flashloader.bin
    COMMAND ${CMAKE_COMMAND} -E copy ${c_SDK_IMAGE_TARGET_DIR}/imgtool_flashloader.bin ${c_SDK_FLOADER_BIN_DIR}/floader_${c_SOC_TYPE}.bin
)
