set(CMAKE_EXECUTABLE_SUFFIX .axf)

set(c_LAYOUT_LD ${c_SOC_PROJECT_DIR}/ameba_layout.ld)
set(c_IMG2_LD ${c_SDK_LD_DIR}/ameba_img2_all.ld)
# note!!! Keep the latest version at the top for release reason.
if(CONFIG_AMEBAGREEN2_B_CUT)
    set(c_ROM_LIB_DIR  ${c_SDK_LIB_DIR}/amebagreen2_rom_bcut)
elseif(CONFIG_AMEBAGREEN2_A_CUT)
    set(c_ROM_LIB_DIR  ${c_SDK_LIB_DIR}/amebagreen2_rom_acut)
endif()


if(CONFIG_LINK_ROM_SYMB)
    set(c_LINK_ROM_LIB)
    if(CONFIG_AMEBAGREEN2_A_CUT)
        set(c_LINK_ROM_SYMBOL ${c_SDK_LD_DIR}/ameba_rom_symbol_acut.ld)
        set(c_LINK_ROM_SYMBOL_OS ${c_SDK_LD_DIR}/ameba_rom_symbol_acut_os.ld)
        set(c_LINK_ROM_SYMBOL_WIFI ${c_SDK_LD_DIR}/ameba_rom_symbol_acut_wifi.ld)
    elseif(CONFIG_AMEBAGREEN2_B_CUT)
        set(c_LINK_ROM_SYMBOL ${c_SDK_LD_DIR}/ameba_rom_symbol_bcut.ld)
        set(c_LINK_ROM_SYMBOL_OS ${c_SDK_LD_DIR}/ameba_rom_symbol_bcut_os.ld)
        set(c_LINK_ROM_SYMBOL_WIFI ${c_SDK_LD_DIR}/ameba_rom_symbol_bcut_wifi.ld)
    endif()
else()
    set(c_LINK_ROM_LIB ${c_ROM_LIB_DIR}/rom.a)
    set(c_LINK_ROM_SYMBOL ${c_SDK_LD_DIR}/ameba_rom_symbol_empty.ld)
endif()

if(CONFIG_FPGA)
    ameba_add_subdirectory_if_exist(make/image_rom_wifi)
endif()

#NOTE: These compile options are not required for rom's component
add_compile_options(-fdata-sections -ffunction-sections)

if (NOT CONFIG_FULLMAC_DEV)
    ameba_add_subdirectory(make/image2)
    add_custom_target(
        ${c_MCU_PROJECT_NAME}
        COMMENT "build ${c_MCU_PROJECT_NAME} only"
        DEPENDS $<TARGET_PROPERTY:${c_MCU_PROJ_CONFIG},image_list>
    )
endif()
