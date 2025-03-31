set(CMAKE_EXECUTABLE_SUFFIX .axf)

add_compile_options(-fdata-sections -ffunction-sections)

ameba_add_subdirectory(make/image2)