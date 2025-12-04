set(c_GLOBAL_COMMON_COMPILE_DEFINES)
set(c_GLOBAL_COMMON_COMPILE_ASM_OPTIONS)
set(c_GLOBAL_COMMON_COMPILE_C_OPTIONS)
set(c_GLOBAL_COMMON_COMPILE_CPP_OPTIONS)
set(c_GLOBAL_MCU_INCLUDE_DIRECTORIES)

# +++++++++++++++++ c_GLOBAL_COMMON_COMPILE_DEFINES ++++++++++++++++ #
ameba_list_append(c_GLOBAL_COMMON_COMPILE_DEFINES
)
# ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #

# +++++++++++++++ c_GLOBAL_COMMON_COMPILE_ASM_OPTIONS ++++++++++++++ #
ameba_list_append(c_GLOBAL_COMMON_COMPILE_ASM_OPTIONS
    -g
    -c
    -MMD -MP
)
# ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #

# ++++++++++++++++ c_GLOBAL_COMMON_COMPILE_C_OPTIONS +++++++++++++++ #
ameba_list_append(c_GLOBAL_COMMON_COMPILE_C_OPTIONS
    ${c_GLOBAL_COMMON_COMPILE_ASM_OPTIONS}
    -nostartfiles
    -nodefaultlibs
    -nostdlib
    # avoid compile optimize based on undefined behavior
    -fno-strict-aliasing -fno-strict-overflow -fno-delete-null-pointer-checks
    -fno-tree-loop-distribute-patterns
    -flax-vector-conversions
    -gdwarf-3
    -fstack-usage

    -Wundef
    -Wpointer-arith
    -Wstrict-prototypes
    -Wno-write-strings
    -Wno-maybe-uninitialized
    -Wall
    -Wextra
    -Werror

    # When the memory length is 4 bytes, memset/memcpy will be optimized for direct 32-bit reading and writing.
    # If the source address is not aligned, an error will result because the hardware does not support unaligned accesses.
    #TODO: -fno-builtin will affect dhrystone and tflite_micro performance.
    -fno-builtin # avoid strcpy(ISO C90) to be optimized to stpcpy(posix) by compiler, including -fno-builtin-memset -fno-builtin-memcpy -fno-builtin-printf...

    -save-temps=obj
    # -fno-short-enums
    # -ffile-prefix-map=${c_BASEDIR}=.
)

if(CONFIG_AMEBALITE OR CONFIG_AMEBAD OR CONFIG_RTL8720F OR CONFIG_AMEBAGREEN2)
    ameba_list_append(c_GLOBAL_COMMON_COMPILE_C_OPTIONS -Os)
else()
    ameba_list_append(c_GLOBAL_COMMON_COMPILE_C_OPTIONS -O2)
endif()
# ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #

# +++++++++++++++ c_GLOBAL_COMMON_COMPILE_CPP_OPTIONS ++++++++++++++ #
ameba_list_append(c_GLOBAL_COMMON_COMPILE_CPP_OPTIONS
    ${c_GLOBAL_COMMON_COMPILE_C_OPTIONS}
    -std=c++11
    -fno-use-cxa-atexit
)
list(REMOVE_ITEM c_GLOBAL_COMMON_COMPILE_CPP_OPTIONS  -Wstrict-prototypes)
# ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #

# ++++++++++++++++ c_GLOBAL_MCU_INCLUDE_DIRECTORIES ++++++++++++++++ #
ameba_list_append(c_GLOBAL_MCU_INCLUDE_DIRECTORIES
    ${c_MENUCONFIG_DIR}/project_${c_MCU_PROJECT_NAME}
    ${c_MCU_PROJECT_DIR}/inc
)
# ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #

# ++++++++++++++++++++++ MATTER MODIFICATIONS ++++++++++++++++++++++ #
if(CONFIG_MATTER_EN)
    message(STATUS "CONFIG_MATTER_EN is ON, modifying global common flags")
    include(${c_CMPT_MATTER_DIR}/project/cmake/flags/compile_options_matter.cmake)
endif()
# ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #
