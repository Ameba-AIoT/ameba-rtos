set(GLOBAL_C_DEFINES)
set(GLOBAL_C_OPTIONS)
set(GLOBAL_CPP_OPTIONS)
set(GLOBAL_ASM_OPTIONS)


list(
    APPEND
    GLOBAL_C_DEFINES
    CONFIG_USE_MBEDTLS_ROM_ALG
    CONFIG_FUNCION_O0_OPTIMIZE
    DM_ODM_SUPPORT_TYPE=32
)

if(CONFIG_AMEBALITE)
    list(APPEND GLOBAL_C_DEFINES __FPU_PRESENT)
endif()

if(CONFIG_AMEBAD)
    list(APPEND GLOBAL_C_DEFINES CONFIG_PLATFORM_8721D)
    list(APPEND GLOBAL_C_OPTIONS -march=armv8-m.main+dsp)
    list(APPEND GLOBAL_ASM_OPTIONS -march=armv8-m.main+dsp)
else()
    list(APPEND GLOBAL_C_OPTIONS -march=armv8.1-m.main+dsp)
    list(APPEND GLOBAL_ASM_OPTIONS -march=armv8.1-m.main+dsp)
endif()

if(CONFIG_AMEBALITE OR CONFIG_AMEBAD)
    list(APPEND GLOBAL_C_OPTIONS -Os)
else()
    list(APPEND GLOBAL_C_OPTIONS -O2)
endif()

if (CONFIG_802154_THREAD_EN)
    list (APPEND GLOBAL_C_DEFINES OPENTHREAD_CONFIG_CORE_USER_CONFIG_HEADER_ENABLE)
endif()

list(
	APPEND  GLOBAL_C_OPTIONS
    -mthumb
    -mcmse
    -g
    -gdwarf-3
    -nodefaultlibs
    -nostdlib
)

if(CONFIG_CPU_HAS_FPU)
    list(
        APPEND GLOBAL_C_OPTIONS
        -mfloat-abi=hard
        -mfpu=fpv5-sp-d16
    )
endif()


list(
    APPEND  GLOBAL_C_OPTIONS
#avoid compile optimize based on undefined behavior
    -fno-strict-aliasing -fno-strict-overflow -fno-delete-null-pointer-checks
    -fno-tree-loop-distribute-patterns
    -flax-vector-conversions

    -gdwarf-3
    -fstack-usage
    -nostartfiles
    -nostdlib
    -Wall
    -Werror
    -Wpointer-arith
    -Wundef
    -Wno-write-strings
    -Wno-maybe-uninitialized
    -save-temps=obj
    -c
    # -MMD -MP

#When the memory length is 4 bytes, memset/memcpy will be optimized for direct 32-bit reading and writing.
#If the source address is not aligned, an error will result because the hardware does not support unaligned accesses.
    -fno-builtin-memset
    -fno-builtin-memcpy
    -fno-builtin-printf

    -Wextra
    -ffile-prefix-map=${BASEDIR}=.
)


list(
    APPEND GLOBAL_CPP_OPTIONS
    ${GLOBAL_C_OPTIONS}
    -std=c++11
    -fno-use-cxa-atexit
)

list(
    APPEND GLOBAL_C_OPTIONS
    -Wstrict-prototypes
)


list(
    APPEND GLOBAL_ASM_OPTIONS
    -g -mthumb -c
    -MMD -MP
)
