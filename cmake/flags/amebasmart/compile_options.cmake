set(d_GLOBAL_PLATFORM_COMPILE_DEFINES)
set(d_GLOBAL_PLATFORM_COMPILE_C_OPTIONS)
set(d_GLOBAL_PLATFORM_COMPILE_CPP_OPTIONS)
set(d_GLOBAL_PLATFORM_COMPILE_ASM_OPTIONS)

# +++++++++++++++++ d_GLOBAL_PLATFORM_COMPILE_DEFINES ++++++++++++++++ #
list(APPEND d_GLOBAL_PLATFORM_COMPILE_DEFINES
    #TODO: add global platform define config here
)

# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #

# ++++++++++++++++ d_GLOBAL_PLATFORM_COMPILE_C_OPTIONS +++++++++++++++ #
list(APPEND d_GLOBAL_PLATFORM_COMPILE_C_OPTIONS
    #TODO: add global platform define config here
)
# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #

# +++++++++++++++ d_GLOBAL_PLATFORM_COMPILE_CPP_OPTIONS ++++++++++++++ #
list(APPEND d_GLOBAL_PLATFORM_COMPILE_CPP_OPTIONS
    #TODO: add global platform compile option for cpp here
    ${d_GLOBAL_PLATFORM_COMPILE_C_OPTIONS}
)
list(REMOVE_ITEM d_GLOBAL_PLATFORM_COMPILE_CPP_OPTIONS  -Wstrict-prototypes)
# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #

# +++++++++++++++ d_GLOBAL_PLATFORM_COMPILE_ASM_OPTIONS ++++++++++++++ #
list(APPEND d_GLOBAL_PLATFORM_COMPILE_ASM_OPTIONS
    #TODO: add global platform compile option for asm here
)
# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ #