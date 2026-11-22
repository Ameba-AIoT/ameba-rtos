set(c_GLOBAL_MCU_LINK_OPTIONS)

# +++++++++++++++++++ c_GLOBAL_MCU_LINK_OPTIONS +++++++++++++++++ #
ameba_list_append(c_GLOBAL_MCU_LINK_OPTIONS
    -mthumb
    -mcmse
)
if(CONFIG_TOOLCHAIN_LLVM)
    # Reset_Handler is PROVIDE'd by the ROM symbol script, ld.lld does not treat ENTRY() as a reference (gnu ld does) and warns
    # "cannot find entry symbol" (and falls back to _start when the script has no ENTRY), -u makes the PROVIDE take effect
    ameba_list_append(c_GLOBAL_MCU_LINK_OPTIONS
        -Wl,-u,Reset_Handler
        -Wl,--entry=Reset_Handler
    )
else()
    # ld.lld has no enum size check, so the option is gcc only
    ameba_list_append(c_GLOBAL_MCU_LINK_OPTIONS -Wl,--no-enum-size-warning)
endif()

if(CONFIG_AMEBAD OR CONFIG_AMEBAPRO3)
    ameba_list_append(c_GLOBAL_MCU_LINK_OPTIONS -march=armv8-m.main+dsp)
elseif(CONFIG_AMEBADPLUS OR CONFIG_AMEBALITE)
    ameba_list_append(c_GLOBAL_MCU_LINK_OPTIONS -march=armv8.1-m.main+dsp)
elseif(CONFIG_AMEBAGREEN2 OR CONFIG_RLE1509)
    ameba_list_append(c_GLOBAL_MCU_LINK_OPTIONS -march=armv8.1-m.main)
elseif(CONFIG_AMEBASMART)
    if(CONFIG_TOOLCHAIN_ASDK_12_3_1 AND CONFIG_CPU_HAS_FPU)
        ameba_list_append(c_GLOBAL_MCU_LINK_OPTIONS -march=armv8.1-m.main+mve.fp)
    else()
        ameba_list_append(c_GLOBAL_MCU_LINK_OPTIONS -march=armv8.1-m.main+mve)
    endif()
else()
    message(FATAL_ERROR "Unsupported soc type")
endif()

ameba_list_append_if(CONFIG_CPU_HAS_FPU c_GLOBAL_MCU_LINK_OPTIONS
    -mfpu=fpv5-sp-d16
    -mfloat-abi=hard
)
# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #
