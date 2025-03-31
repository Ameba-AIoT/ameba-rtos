set(c_GLOBAL_MCU_LINK_OPTIONS)

# +++++++++++++++++++ c_GLOBAL_MCU_LINK_OPTIONS +++++++++++++++++ #
ameba_list_append(c_GLOBAL_MCU_LINK_OPTIONS
    -L${SDK_TOOLCHAIN}/arm-none-eabi/lib/thumb/v8-m.base
    -L${SDK_TOOLCHAIN}/lib/gcc/arm-none-eabi/${ASDK_VER}/thumb/v8-m.base
    -march=armv8-m.base
    "SHELL:-specs nano.specs"
    -mthumb
    -Wl,--no-enum-size-warning
)
# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #
