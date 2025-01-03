set(d_GLOBAL_MCU_COMPILE_DEFINES)
set(d_GLOBAL_MCU_COMPILE_C_OPTIONS)
set(d_GLOBAL_MCU_COMPILE_CPP_OPTIONS)
set(d_GLOBAL_MCU_COMPILE_ASM_OPTIONS)

# +++++++++++++++++ d_GLOBAL_MCU_COMPILE_DEFINES ++++++++++++++++ #
list(APPEND d_GLOBAL_MCU_COMPILE_DEFINES
    #TODO: add global mcu define config here
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

# ++++++++++++++++ d_GLOBAL_MCU_COMPILE_C_OPTIONS +++++++++++++++ #
list(APPEND d_GLOBAL_MCU_COMPILE_C_OPTIONS
    -mcpu=cortex-a32
    -mfpu=neon
    -mfloat-abi=hard
    -marm
    -fno-jump-tables #For link _thumb1_case_uqi issue
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
    -mcpu=cortex-a32
    -mfpu=vfpv4
    -mfloat-abi=hard
)

if(CONFIG_DYNAMIC_APP_LOAD_EN)
    list(APPEND d_GLOBAL_MCU_COMPILE_C_OPTIONS
        -ffixed-r9
    )
    list(APPEND d_GLOBAL_MCU_COMPILE_ASM_OPTIONS
        -ffixed-r9
    )
endif()
# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #


