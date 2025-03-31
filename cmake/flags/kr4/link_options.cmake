set(c_GLOBAL_MCU_LINK_OPTIONS)

# +++++++++++++++++++ c_GLOBAL_MCU_LINK_OPTIONS +++++++++++++++++ #
ameba_list_append(c_GLOBAL_MCU_LINK_OPTIONS
    -L${SDK_TOOLCHAIN}/riscv32-none-elf/lib/rv32imafc/ilp32f
    -L${SDK_TOOLCHAIN}/lib/gcc/riscv32-none-elf/${VSDK_VER}/rv32imafc/ilp32f
    -Os
    -march=rv32imafc -mabi=ilp32f
    -msafe-relax
    -Wl,--start-group -lc -lgcc  -Wl,--end-group
)
# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #
