set(GLOBAL_C_DEFINES)
set(GLOBAL_C_OPTIONS)
set(GLOBAL_CPP_OPTIONS)
set(GLOBAL_ASM_OPTIONS)


list(
    APPEND
    GLOBAL_C_DEFINES
    RISC_V
    CONFIG_USE_MBEDTLS_ROM_ALG
    CONFIG_FUNCION_O0_OPTIMIZE
    DM_ODM_SUPPORT_TYPE=32
    MBEDTLS_CONFIG_FILE="mbedtls/config.h"
)

list(
	APPEND  GLOBAL_C_OPTIONS
    -march=rv32imafc_xrtkcsh1p0
    -mabi=ilp32f
    -Os
    -g
    -msafe-relax
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
    -fno-builtin-printf
    -Wall
    -Werror
    -Wpointer-arith
    -Wundef
    -Wno-write-strings
    -Wno-maybe-uninitialized
    -save-temps=obj
    -c
    -fshort-enums
    -mstrict-align
    # -MMD -MP

#When the memory length is 4 bytes, memset/memcpy will be optimized for direct 32-bit reading and writing.
#If the source address is not aligned, an error will result because the hardware does not support unaligned accesses.
    -fno-builtin-memset
    -fno-builtin-memcpy

    -Wextra
)


list(
    APPEND GLOBAL_CPP_OPTIONS
    ${GLOBAL_C_OPTIONS}
    -fno-use-cxa-atexit
)

list(
    APPEND GLOBAL_C_OPTIONS
    -Wstrict-prototypes
)


list(
    APPEND GLOBAL_ASM_OPTIONS
    -g -march=rv32imafc_xrtkcsh1p0 -mabi=ilp32f -c
    -MMD -MP
)
