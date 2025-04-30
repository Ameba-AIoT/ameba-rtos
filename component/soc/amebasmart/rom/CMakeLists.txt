if(${c_CURRENT_IMAGE_IS_ROM})
ameba_fatal("For CA32 Only, ROM cannot call ${CMAKE_CURRENT_LIST_DIR}")
endif()

ameba_add_subdirectory(rom_hp)
ameba_add_subdirectory(rom_common)
ameba_add_subdirectory(stdlib)
ameba_add_subdirectory(string)

ameba_add_merge_soc_library(rom
    rom_hp
    rom_common
    rom_stdlib
    rom_string
)
