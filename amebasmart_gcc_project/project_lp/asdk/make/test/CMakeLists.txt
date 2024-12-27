ameba_add_subdirectories_if(CONFIG_PER_TEST
    verification
    std_lib
)
ameba_add_subdirectories_if(CONFIG_DHRYSTONE_TEST
    coremark
    dhrystone
)