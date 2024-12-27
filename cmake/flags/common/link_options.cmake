set(d_GLOBAL_COMMON_LINK_OPTIONS)

# +++++++++++++++++++ d_GLOBAL_COMMON_LINK_OPTIONS +++++++++++++++++ #
list(APPEND d_GLOBAL_COMMON_LINK_OPTIONS
    -O2
    -mcmse
    -Wl,-Map=text.map
    -nostartfiles
    "SHELL:-specs nosys.specs"
    -Wl,--gc-sections
    -Wl,-Map=text.map
    -Wl,--cref
    -Wl,--build-id=none
    -save-temps
    -Wl,--no-enum-size-warning
    -Wl,--warn-common
    -L${d_MCU_SDK_DIR}/lib/
)

# ameba_list_append_ifnot(ROM_CFLAG d_GLOBAL_COMMON_LINK_OPTIONS "-Wl,--gc-sections")
# ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #
