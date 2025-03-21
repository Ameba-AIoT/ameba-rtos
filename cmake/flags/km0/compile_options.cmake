set(d_GLOBAL_MCU_COMPILE_DEFINES)
set(d_GLOBAL_MCU_COMPILE_C_OPTIONS)
set(d_GLOBAL_MCU_COMPILE_CPP_OPTIONS)
set(d_GLOBAL_MCU_COMPILE_ASM_OPTIONS)

# +++++++++++++++++ d_GLOBAL_MCU_COMPILE_DEFINES ++++++++++++++++ #
list(APPEND d_GLOBAL_MCU_COMPILE_DEFINES
    #TODO: add global mcu define config here
    # "__ARM_FEATURE_DSP=0"
)
# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #

# ++++++++++++++++ d_GLOBAL_MCU_COMPILE_C_OPTIONS +++++++++++++++ #
list(APPEND d_GLOBAL_MCU_COMPILE_C_OPTIONS
    #TODO: add global mcu compile option for c here
    -march=armv8-m.base
    -mthumb
)
# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #

# +++++++++++++++ d_GLOBAL_MCU_COMPILE_CPP_OPTIONS ++++++++++++++ #
list(APPEND d_GLOBAL_MCU_COMPILE_CPP_OPTIONS
    #TODO: add global mcu compile option for cpp here
    ${d_GLOBAL_MCU_COMPILE_C_OPTIONS}
)
list(REMOVE_ITEM d_GLOBAL_MCU_COMPILE_CPP_OPTIONS  -Wstrict-prototypes)
# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #

# +++++++++++++++ d_GLOBAL_MCU_COMPILE_ASM_OPTIONS ++++++++++++++ #
list(APPEND d_GLOBAL_MCU_COMPILE_ASM_OPTIONS
    #TODO: add global mcu compile option for asm here
    -march=armv8-m.base
    -mthumb
)
# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #


