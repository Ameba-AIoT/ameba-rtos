set(GLOBAL_C_DEFINES)
set(GLOBAL_C_OPTIONS)
set(GLOBAL_ASM_OPTIONS)


list(
    APPEND
    GLOBAL_C_DEFINES
    __ARM_FEATURE_DSP=0
    DM_ODM_SUPPORT_TYPE=32
)

if(CONFIG_AMEBAD)
    list(APPEND GLOBAL_C_DEFINES CONFIG_PLATFORM_8721D)
    list(APPEND GLOBAL_C_OPTIONS -Os)
else()
    list(APPEND GLOBAL_C_OPTIONS -O2)
endif()

list(
	APPEND  GLOBAL_C_OPTIONS
    -g
    #avoid compile optimize based on undefined behavior
    -fno-strict-aliasing
    -fno-strict-overflow
    -fno-delete-null-pointer-checks
    #-specs nosys.specs
    -march=armv8-m.base
    -mthumb

    -gdwarf-3
    -fstack-usage

    -nostartfiles
    -nostdlib
    -fno-builtin-printf
    -Wall
    -Werror
    -Wpointer-arith
    -Wstrict-prototypes
    -Wundef
    -Wno-write-strings
    -Wno-maybe-uninitialized
    -save-temps=obj
    -c
    # -MMD -MP

    -fno-jump-tables #For link _thumb1_case_uqi issue
    -fno-tree-loop-distribute-patterns

    #When the memory length is 4 bytes, memset/memcpy will be optimized for direct 32-bit reading and writing.
    #If the source address is not aligned, an error will result because the hardware does not support unaligned accesses.
    -fno-builtin-memset
    -fno-builtin-memcpy

    #-fno-short-enums
    -Wextra
    #-Wconversion
)


list(
    APPEND GLOBAL_ASM_OPTIONS
    -g -march=armv8-m.base -mthumb -c
    -MMD -MP
)
