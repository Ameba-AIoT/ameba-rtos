set(d_GLOBAL_MCU_LINK_OPTIONS)

# +++++++++++++++++++ d_GLOBAL_MCU_LINK_OPTIONS +++++++++++++++++ #
list(APPEND d_GLOBAL_MCU_LINK_OPTIONS
    #TODO: add global mcu link options here
    -marm
    -mcpu=cortex-a32
    -mfpu=vfpv4
    -mfloat-abi=hard
    -L${d_TOOLCHAIN_DIR}/arm-none-eabi/lib/v7-a+neon-fp16/fpu/
    -L${d_TOOLCHAIN_DIR}/lib/gcc/arm-none-eabi/${d_SDK_VERSION}/v7-a+neon-fp16/fpu
)

# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #