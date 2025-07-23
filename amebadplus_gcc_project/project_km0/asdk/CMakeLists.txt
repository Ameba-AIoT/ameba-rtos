set(CMAKE_EXECUTABLE_SUFFIX .axf)
add_compile_options(-fdata-sections -ffunction-sections)

set(c_LAYOUT_LD ${c_SOC_PROJECT_DIR}/ameba_layout.ld)
if(CONFIG_MP_SHRINK)
    set(c_IMG2_LD ${c_SDK_LD_DIR}/ameba_img2_all_mp.ld)
else()
    set(c_IMG2_LD ${c_SDK_LD_DIR}/ameba_img2_all.ld)
endif()

if(CONFIG_AMEBADPLUS_A_CUT)
    ameba_set(c_ROM_LIB_DIR ${c_SDK_LIB_DIR}/amebadplus_rom_acut)
endif()

if(CONFIG_LINK_ROM_SYMB)
    set(c_LINK_ROM_LIB)
    set(c_LINK_ROM_SYMBOL ${c_SDK_LD_DIR}/ameba_rom_symbol_acut.ld)
else()
    set(c_LINK_ROM_LIB ${c_ROM_LIB_DIR}/rom.a)
    set(c_LINK_ROM_SYMBOL ${c_SDK_LD_DIR}/ameba_rom_symbol_empty.ld)
endif()

list(APPEND c_LINK_ROM_SYMBOL ${c_SDK_LD_DIR}/ameba_rom_symbol_stdlib.ld)


ameba_add_subdirectory(make/image2)

add_custom_target(
    ${c_MCU_PROJECT_NAME}
    COMMENT "build ${c_MCU_PROJECT_NAME} only"
    DEPENDS $<TARGET_PROPERTY:${c_MCU_PROJ_CONFIG},image_list>
)