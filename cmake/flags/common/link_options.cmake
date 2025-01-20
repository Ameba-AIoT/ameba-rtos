set(d_GLOBAL_COMMON_LINK_OPTIONS)

# +++++++++++++++++++ d_GLOBAL_COMMON_LINK_OPTIONS +++++++++++++++++ #
list(APPEND d_GLOBAL_COMMON_LINK_OPTIONS
    -O2
    -mcmse
    -Wl,-Map=text.map
    -nostartfiles
    "SHELL:-specs nosys.specs"
    # Add -Wl,--gc-sections in actual target for rom compile.
    -Wl,-Map=text.map
    -Wl,--cref
    -Wl,--build-id=none
    -save-temps
    -Wl,--no-enum-size-warning
    -Wl,--warn-common
    -L${d_MCU_SDK_DIR}/lib/
)

# ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #
