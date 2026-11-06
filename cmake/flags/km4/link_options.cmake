set(c_GLOBAL_MCU_LINK_OPTIONS)

# +++++++++++++++++++ c_GLOBAL_MCU_LINK_OPTIONS +++++++++++++++++ #
ameba_list_append(c_GLOBAL_MCU_LINK_OPTIONS
    -mthumb
    -mcmse
    -Wl,--no-enum-size-warning
)

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
