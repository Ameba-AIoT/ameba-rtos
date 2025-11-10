set(CMAKE_EXECUTABLE_SUFFIX .axf)

add_compile_options(-fdata-sections -ffunction-sections)
# note!!! Keep the latest version(ap:A) at the top for release reason.
ameba_set(c_LINK_ROM_LIB ${c_SDK_LIB_SOC_DIR}/lib_rom.a)

ameba_add_subdirectory(make/image2)
