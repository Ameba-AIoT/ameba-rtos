set(c_GLOBAL_MCU_COMPILE_DEFINES)
set(c_GLOBAL_MCU_COMPILE_ASM_OPTIONS)
set(c_GLOBAL_MCU_COMPILE_C_OPTIONS)
set(c_GLOBAL_MCU_COMPILE_CPP_OPTIONS)

# +++++++++++++++++ c_GLOBAL_MCU_COMPILE_DEFINES ++++++++++++++++ #
ameba_list_append(c_GLOBAL_MCU_COMPILE_DEFINES
    RISC_V
)
# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #

# +++++++++++++++ c_GLOBAL_MCU_COMPILE_ASM_OPTIONS ++++++++++++++ #
ameba_list_append(c_GLOBAL_MCU_COMPILE_ASM_OPTIONS
    -march=rv32imafc_xrtkcsh1p0
    -mabi=ilp32f
)
# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #

# ++++++++++++++++ c_GLOBAL_MCU_COMPILE_C_OPTIONS +++++++++++++++ #
ameba_list_append(c_GLOBAL_MCU_COMPILE_C_OPTIONS
    ${c_GLOBAL_MCU_COMPILE_ASM_OPTIONS}
    -Os
    -msafe-relax
    -fshort-enums
    -mstrict-align
)
# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #

# +++++++++++++++ c_GLOBAL_MCU_COMPILE_CPP_OPTIONS ++++++++++++++ #
ameba_list_append(c_GLOBAL_MCU_COMPILE_CPP_OPTIONS
    ${c_GLOBAL_MCU_COMPILE_C_OPTIONS}
)
# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #
