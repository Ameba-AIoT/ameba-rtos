set(c_GLOBAL_MCU_LINK_OPTIONS)

# +++++++++++++++++++ c_GLOBAL_MCU_LINK_OPTIONS +++++++++++++++++ #
ameba_list_append(c_GLOBAL_MCU_LINK_OPTIONS
    -marm
    -mcpu=cortex-a32
    -mfpu=neon
    -mfloat-abi=hard
    -L${SDK_TOOLCHAIN}/arm-none-eabi/lib/v7-a+neon-fp16/fpu/
    -L${SDK_TOOLCHAIN}/lib/gcc/arm-none-eabi/${c_SDK_VERSION}/v7-a+neon-fp16/fpu
)
# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #
