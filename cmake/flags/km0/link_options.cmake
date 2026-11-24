set(c_GLOBAL_MCU_LINK_OPTIONS)

# +++++++++++++++++++ c_GLOBAL_MCU_LINK_OPTIONS +++++++++++++++++ #
if(CONFIG_TOOLCHAIN_LLVM)
    ameba_list_append(c_GLOBAL_MCU_LINK_OPTIONS
        -march=armv8-m.base
        -mfloat-abi=soft # so that clang selects the thumb/v8-m.base multilib
        -mthumb
        # Reset_Handler is PROVIDE'd by the ROM symbol script, ld.lld does not treat ENTRY() as a reference (gnu ld does) and warns
        # "cannot find entry symbol", -u makes the PROVIDE take effect
        -Wl,-u,Reset_Handler
        -Wl,--entry=Reset_Handler
    )
else()
    ameba_list_append(c_GLOBAL_MCU_LINK_OPTIONS
        -L${SDK_TOOLCHAIN}/arm-none-eabi/lib/thumb/v8-m.base
        -L${SDK_TOOLCHAIN}/lib/gcc/arm-none-eabi/${ASDK_VER}/thumb/v8-m.base
        -march=armv8-m.base
        "SHELL:-specs nano.specs"
        -mthumb
        -Wl,--no-enum-size-warning
    )
endif()
# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #
