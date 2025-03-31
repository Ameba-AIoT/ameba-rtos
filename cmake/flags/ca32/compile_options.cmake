set(c_GLOBAL_MCU_COMPILE_DEFINES)
set(c_GLOBAL_MCU_COMPILE_ASM_OPTIONS)
set(c_GLOBAL_MCU_COMPILE_C_OPTIONS)
set(c_GLOBAL_MCU_COMPILE_CPP_OPTIONS)

# +++++++++++++++++ c_GLOBAL_MCU_COMPILE_DEFINES ++++++++++++++++ #
ameba_list_append(c_GLOBAL_MCU_COMPILE_DEFINES
    #TODO: why global mcu define config here?
    CONFIG_GIC_VER=${v_GIC_VER}
    configINTERRUPT_CONTROLLER_VERSION=${v_GIC_VER}
    CONFIG_CPUS_NUM=${CONFIG_CORE_NUM}
    DEBUG

    ARM_ARCH_MAJOR=7
    LOG_LEVEL=40
    ARMV7_SUPPORTS_LARGE_PAGE_ADDRESSING=1
    PLAT_RO_XLAT_TABLES=0
    PLAT_XLAT_TABLES_DYNAMIC=0
    ENABLE_BTI=0
    HW_ASSISTED_COHERENCY=0
    ENABLE_ASSERTIONS=1
    WARMBOOT_ENABLE_DCACHE_EARLY=0
)
# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #

# +++++++++++++++ c_GLOBAL_MCU_COMPILE_ASM_OPTIONS ++++++++++++++ #
ameba_list_append(c_GLOBAL_MCU_COMPILE_ASM_OPTIONS
    -mcpu=cortex-a32
    -mfpu=neon
    -mfloat-abi=hard
)

ameba_list_append_if(CONFIG_DYNAMIC_APP_LOAD_EN c_GLOBAL_MCU_COMPILE_ASM_OPTIONS
    -ffixed-r9
)
# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #

# ++++++++++++++++ c_GLOBAL_MCU_COMPILE_C_OPTIONS +++++++++++++++ #
ameba_list_append(c_GLOBAL_MCU_COMPILE_C_OPTIONS
    ${c_GLOBAL_MCU_COMPILE_ASM_OPTIONS}
    -marm
)
# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #

# +++++++++++++++ c_GLOBAL_MCU_COMPILE_CPP_OPTIONS ++++++++++++++ #
ameba_list_append(c_GLOBAL_MCU_COMPILE_CPP_OPTIONS
    ${c_GLOBAL_MCU_COMPILE_C_OPTIONS}
)
# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #
