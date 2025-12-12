if(CONFIG_AMEBA_RLS)
    message(FATAL_ERROR "This file MUST NOT be released")
endif()

function(ameba_library_section_rename target)
    set(options
        p_TO_ROM
        p_TO_COM
        p_TO_SHARE
        p_TO_SSL
        p_TO_MVE
    )

    cmake_parse_arguments(ARG "${options}" "" "" ${ARGN})

    if(ARG_p_TO_ROM)
        add_custom_command(
            TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} --rename-section .rodata.str1.4=.rom.rodata.str1.4 $<TARGET_FILE:${target}> $<TARGET_FILE:${target}>
            COMMAND ${CMAKE_OBJCOPY} --rename-section .rodata.str1.1=.rom.rodata.str1.1 $<TARGET_FILE:${target}> $<TARGET_FILE:${target}>
            COMMAND ${CMAKE_OBJCOPY} --rename-section .rodata=.rom.rodata $<TARGET_FILE:${target}> $<TARGET_FILE:${target}>
            COMMENT "RENAME_ROM_OBJS"
        )
    endif()

    if(ARG_p_TO_COM)
        add_custom_command(
            TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} --rename-section .hal.rom.rodata=.com.rom.rodata.hal $<TARGET_FILE:${target}> $<TARGET_FILE:${target}>
            COMMAND ${CMAKE_OBJCOPY} --rename-section .rodata.str1.4=.com.rom.rodata.str1.4 $<TARGET_FILE:${target}> $<TARGET_FILE:${target}>
            COMMAND ${CMAKE_OBJCOPY} --rename-section .rodata.str1.1=.com.rom.rodata.str1.1 $<TARGET_FILE:${target}> $<TARGET_FILE:${target}>
            COMMAND ${CMAKE_OBJCOPY} --rename-section .rodata=.com.rom.rodata $<TARGET_FILE:${target}> $<TARGET_FILE:${target}>
            COMMENT "MERGE_ROM_RODATA_COMMON"
        )
    endif()

    if(ARG_p_TO_SHARE)
        add_custom_command(
            TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} --rename-section .hal.rom.text=.share.rom.text $<TARGET_FILE:${target}> $<TARGET_FILE:${target}>
            COMMAND ${CMAKE_OBJCOPY} --rename-section .hal.rom.rodata=.share.rom.rodata.hal $<TARGET_FILE:${target}> $<TARGET_FILE:${target}>
            COMMAND ${CMAKE_OBJCOPY} --rename-section .rodata.str1.4=.share.rom.rodata.str1.4 $<TARGET_FILE:${target}> $<TARGET_FILE:${target}>
            COMMAND ${CMAKE_OBJCOPY} --rename-section .rodata.str1.1=.share.rom.rodata.str1.1 $<TARGET_FILE:${target}> $<TARGET_FILE:${target}>
            COMMAND ${CMAKE_OBJCOPY} --rename-section .rodata=.share.rom.rodata $<TARGET_FILE:${target}> $<TARGET_FILE:${target}>
            COMMENT "ROM_COMMON_SHARE"
        )
    endif()

    if(ARG_p_TO_SSL)
        add_custom_command(
            TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} --rename-section .text=.ssl.rom.text $<TARGET_FILE:${target}> $<TARGET_FILE:${target}>
            COMMAND ${CMAKE_OBJCOPY} --rename-section .rodata=.ssl.rom.rodata $<TARGET_FILE:${target}> $<TARGET_FILE:${target}>
            COMMAND ${CMAKE_OBJCOPY} --rename-section .rodata.str1.4=.ssl.rom.rodata.str1.4 $<TARGET_FILE:${target}> $<TARGET_FILE:${target}>
            COMMAND ${CMAKE_OBJCOPY} --rename-section .rodata.str1.1=.ssl.rom.rodata.str1.1 $<TARGET_FILE:${target}> $<TARGET_FILE:${target}>
            COMMAND ${CMAKE_OBJCOPY} --rename-section .hal.rom.bss=.hal.rom.bank.bss $<TARGET_FILE:${target}> $<TARGET_FILE:${target}>
            COMMENT "RENAME_SSL2ROM"
        )
    endif()

    if(ARG_p_TO_MVE)
        add_custom_command(
            TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} --rename-section .text=.mve.rom.text $<TARGET_FILE:${target}> $<TARGET_FILE:${target}>
            COMMENT "RENAME_MVE_OBJS"
        )
    endif()
endfunction()

function(ameba_library_section_prefix target prefix)
    add_custom_command(
        TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} --prefix-alloc-sections ${prefix} $<TARGET_FILE:${target}>
        COMMENT "add prefix to sections of ${target} with prefix ${prefix}"
    )
endfunction()
