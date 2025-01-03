set(d_GLOBAL_COMMON_COMPILE_DEFINES)
set(d_GLOBAL_COMMON_COMPILE_C_OPTIONS)
set(d_GLOBAL_COMMON_COMPILE_CPP_OPTIONS)
set(d_GLOBAL_COMMON_COMPILE_ASM_OPTIONS)
set(d_GLOBAL_MCU_INCLUDE_DIRECTORIES)

# +++++++++++++++++ d_GLOBAL_COMMON_COMPILE_DEFINES ++++++++++++++++ #
list(APPEND d_GLOBAL_COMMON_COMPILE_DEFINES
    CONFIG_PLATFORM_${d_PLATFORM_TYPE_UPPER}
    CONFIG_USE_MBEDTLS_ROM_ALG
    CONFIG_FUNCION_O0_OPTIMIZE
    DM_ODM_SUPPORT_TYPE=32
)

ameba_list_append_if(IMAGE2_CFLAGS d_GLOBAL_COMMON_COMPILE_DEFINES IMAGE2_BUILD)
# ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #

# ++++++++++++++++ d_GLOBAL_COMMON_COMPILE_C_OPTIONS +++++++++++++++ #
list(APPEND d_GLOBAL_COMMON_COMPILE_C_OPTIONS
    -g
    -nostartfiles
    -nodefaultlibs
    -nostdlib
    -fno-builtin-printf
    -O2
    -fno-strict-aliasing
    -fno-strict-overflow
    -fno-delete-null-pointer-checks
    -fno-tree-loop-distribute-patterns
    -flax-vector-conversions
    -gdwarf-3
    -fstack-usage
    -Wall
    -Werror
    -Wpointer-arith

    #When the memory length is 4 bytes, memset/memcpy will be optimized for direct 32-bit reading and writing.
    #If the source address is not aligned, an error will result because the hardware does not support unaligned accesses.
    -fno-builtin-memset
    -fno-builtin-memcpy

    -Wundef
    -Wno-write-strings
    -Wno-maybe-uninitialized
    -save-temps=obj
    -c
    -MMD
    -MP
    #-fno-short-enums
    -Wextra

    # -ffile-prefix-map=${c_BASEDIR}=.
    -Wstrict-prototypes
    -fdata-sections
    -ffunction-sections
)

# ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #


# +++++++++++++++ d_GLOBAL_COMMON_COMPILE_CPP_OPTIONS ++++++++++++++ #
list(APPEND d_GLOBAL_COMMON_COMPILE_CPP_OPTIONS
    ${d_GLOBAL_COMMON_COMPILE_C_OPTIONS}
    -std=c++11
    -fno-use-cxa-atexit
)
list(REMOVE_ITEM d_GLOBAL_COMMON_COMPILE_CPP_OPTIONS  -Wstrict-prototypes)
# ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #


# +++++++++++++++ d_GLOBAL_COMMON_COMPILE_ASM_OPTIONS ++++++++++++++ #
list(APPEND d_GLOBAL_COMMON_COMPILE_ASM_OPTIONS
    -g
    -c
    -MMD
    -MP
)
# ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #




# ++++++++++++++++ d_GLOBAL_MCU_INCLUDE_DIRECTORIES ++++++++++++++++ #
list(APPEND d_GLOBAL_MCU_INCLUDE_DIRECTORIES
    ${d_MCU_PROJECT_DIR}/inc
)
# ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #
