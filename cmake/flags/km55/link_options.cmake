set(c_GLOBAL_MCU_LINK_OPTIONS)

# +++++++++++++++++++ c_GLOBAL_MCU_LINK_OPTIONS +++++++++++++++++ #
ameba_list_append(c_GLOBAL_MCU_LINK_OPTIONS
    -mthumb
    -mcmse
    -Wl,--no-enum-size-warning
)

if(CONFIG_RTL8720F)
    ameba_list_append(c_GLOBAL_MCU_LINK_OPTIONS -march=armv8.1-m.main)
else()
    message(FATAL_ERROR "Unsupported soc type")
endif()

ameba_list_append_if(CONFIG_CPU_HAS_FPU c_GLOBAL_MCU_LINK_OPTIONS
    -mfpu=fpv5-sp-d16
    -mfloat-abi=hard
)
# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #
