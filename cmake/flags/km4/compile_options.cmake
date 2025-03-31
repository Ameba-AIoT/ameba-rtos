set(c_GLOBAL_MCU_COMPILE_DEFINES)
set(c_GLOBAL_MCU_COMPILE_ASM_OPTIONS)
set(c_GLOBAL_MCU_COMPILE_C_OPTIONS)
set(c_GLOBAL_MCU_COMPILE_CPP_OPTIONS)

# +++++++++++++++++ c_GLOBAL_MCU_COMPILE_DEFINES ++++++++++++++++ #
ameba_list_append(c_GLOBAL_MCU_COMPILE_DEFINES
    #TODO: to be removed when new cmake is ready
    MBEDTLS_CONFIG_FILE="mbedtls/config.h"
)
# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #

# +++++++++++++++ c_GLOBAL_MCU_COMPILE_ASM_OPTIONS ++++++++++++++ #
ameba_list_append(c_GLOBAL_MCU_COMPILE_ASM_OPTIONS
    -mthumb
)

if(CONFIG_AMEBAD)
    ameba_list_append(c_GLOBAL_MCU_COMPILE_ASM_OPTIONS -march=armv8-m.main+dsp)
elseif(CONFIG_AMEBADPLUS OR CONFIG_AMEBALITE)
    ameba_list_append(c_GLOBAL_MCU_COMPILE_ASM_OPTIONS -march=armv8.1-m.main+dsp)
elseif(CONFIG_AMEBAGREEN2 OR CONFIG_AMEBAL2)
    ameba_list_append(c_GLOBAL_MCU_COMPILE_ASM_OPTIONS -march=armv8.1-m.main)
elseif(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
    ameba_list_append(c_GLOBAL_MCU_COMPILE_ASM_OPTIONS -march=armv8.1-m.main+mve)
else()
    message(FATAL_ERROR "Unsupported soc type")
endif()
# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #

# ++++++++++++++++ c_GLOBAL_MCU_COMPILE_C_OPTIONS +++++++++++++++ #
ameba_list_append(c_GLOBAL_MCU_COMPILE_C_OPTIONS
    ${c_GLOBAL_MCU_COMPILE_ASM_OPTIONS}
    -mcmse
)

ameba_list_append_if(CONFIG_CPU_HAS_FPU c_GLOBAL_MCU_COMPILE_C_OPTIONS
    -mfloat-abi=hard
    -mfpu=fpv5-sp-d16
)
# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #

# +++++++++++++++ c_GLOBAL_MCU_COMPILE_CPP_OPTIONS ++++++++++++++ #
ameba_list_append(c_GLOBAL_MCU_COMPILE_CPP_OPTIONS
    ${c_GLOBAL_MCU_COMPILE_C_OPTIONS}
)
# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #
