set(CMAKE_EXECUTABLE_SUFFIX .axf)

add_compile_options(-fdata-sections -ffunction-sections)
ameba_set(C_LINK_ROM_LIB ${c_SDK_LIB_SOC_DIR}/lib_rom_hp.a)

ameba_add_subdirectory(make/image2)