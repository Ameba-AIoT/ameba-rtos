set(d_GLOBAL_MCU_LINK_OPTIONS)

# +++++++++++++++++++ d_GLOBAL_MCU_LINK_OPTIONS +++++++++++++++++ #
list(APPEND d_GLOBAL_MCU_LINK_OPTIONS
    #TODO: add global mcu link options here
    -L${SDK_TOOLCHAIN}/arm-none-eabi/lib/thumb/v8-m.main+fp/fpu
    -L${SDK_TOOLCHAIN}/lib/gcc/arm-none-eabi/${d_SDK_VERSION}/thumb/v8-m.main+fp/fpu
    -march=armv8.1-m.main+mve
    -mfpu=fpv5-sp-d16
    -mfloat-abi=hard
    -mthumb
)
# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #
