set(c_GLOBAL_MCU_LINK_OPTIONS)

# +++++++++++++++++++ c_GLOBAL_MCU_LINK_OPTIONS +++++++++++++++++ #
ameba_list_append(c_GLOBAL_MCU_LINK_OPTIONS
    -mthumb
    -mcmse
    -Wl,--no-enum-size-warning
)

if(CONFIG_AMEBAD)
    ameba_list_append(c_GLOBAL_MCU_LINK_OPTIONS -march=armv8-m.main+dsp)
elseif(CONFIG_AMEBADPLUS OR CONFIG_AMEBALITE)
    ameba_list_append(c_GLOBAL_MCU_LINK_OPTIONS -march=armv8.1-m.main+dsp)
elseif(CONFIG_AMEBAGREEN2 OR CONFIG_AMEBAL2)
    ameba_list_append(c_GLOBAL_MCU_LINK_OPTIONS -march=armv8.1-m.main)
elseif(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
    ameba_list_append(c_GLOBAL_MCU_LINK_OPTIONS -march=armv8.1-m.main+mve)
else()
    message(FATAL_ERROR "Unsupported soc type")
endif()

ameba_list_append_if(CONFIG_CPU_HAS_FPU c_GLOBAL_MCU_LINK_OPTIONS
    -L${SDK_TOOLCHAIN}/arm-none-eabi/lib/thumb/v8-m.main+fp/fpu
    -L${SDK_TOOLCHAIN}/lib/gcc/arm-none-eabi/${ASDK_VER}/thumb/v8-m.main+fp/fpu
    -mfpu=fpv5-sp-d16
    -mfloat-abi=hard
)
# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #
