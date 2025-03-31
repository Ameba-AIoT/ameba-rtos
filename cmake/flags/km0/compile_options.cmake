set(c_GLOBAL_MCU_COMPILE_DEFINES)
set(c_GLOBAL_MCU_COMPILE_ASM_OPTIONS)
set(c_GLOBAL_MCU_COMPILE_C_OPTIONS)
set(c_GLOBAL_MCU_COMPILE_CPP_OPTIONS)

# +++++++++++++++++ c_GLOBAL_MCU_COMPILE_DEFINES ++++++++++++++++ #
ameba_list_append(c_GLOBAL_MCU_COMPILE_DEFINES
)
# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #

# +++++++++++++++ c_GLOBAL_MCU_COMPILE_ASM_OPTIONS ++++++++++++++ #
ameba_list_append(c_GLOBAL_MCU_COMPILE_ASM_OPTIONS
    -march=armv8-m.base
    -mthumb
)
# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #

# ++++++++++++++++ c_GLOBAL_MCU_COMPILE_C_OPTIONS +++++++++++++++ #
ameba_list_append(c_GLOBAL_MCU_COMPILE_C_OPTIONS
    ${c_GLOBAL_MCU_COMPILE_ASM_OPTIONS}
    -fno-jump-tables # For link _thumb1_case_uqi issue, avoid switch jump table to save space, maybe only need for amebasmart.
)
# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #

# +++++++++++++++ c_GLOBAL_MCU_COMPILE_CPP_OPTIONS ++++++++++++++ #
ameba_list_append(c_GLOBAL_MCU_COMPILE_CPP_OPTIONS
    ${c_GLOBAL_MCU_COMPILE_C_OPTIONS}
)
# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #
