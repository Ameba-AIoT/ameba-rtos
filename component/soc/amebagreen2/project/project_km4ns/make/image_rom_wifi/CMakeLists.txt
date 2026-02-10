add_compile_options(
    -ffile-prefix-map=${c_BASEDIR}=./
    -DCONFIG_BUILD_ROM
)

set(ROM_SYMBOL_GEN_PYTHON  python ${c_SDK_ROM_WIFI_SYMBOL_GEN_SCRIPT} ${CMAKE_READELF} ${c_ROM_LIB_DIR}/target_rom_wifi.axf)
set(TOTAL_SIZE_PYTHON python ${c_SDK_ROM_TOTAL_SIZE_SCRIPT} ${c_ROM_LIB_DIR}/rom_wifi_size.txt)
set(CODE_ANALYZE_PYTHON python ${c_SDK_ROM_CODE_ANALYZE_SCRIPT})

add_subdirectory(wifi_driver)


add_custom_target(
    rom_wifi_chk
    ALL
    COMMAND ${CMAKE_COMMAND} -E echo "build rom"
    COMMENT "build rom"
    DEPENDS
        $<TARGET_PROPERTY:${c_MCU_PROJ_CONFIG},rom_ns>_postbuild
)
