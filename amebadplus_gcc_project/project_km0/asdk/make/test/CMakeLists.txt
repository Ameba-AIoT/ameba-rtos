if(CONFIG_DHRYSTONE_TEST)
    add_subdirectory(coremark)
    add_subdirectory(dhrystone)
endif()

if(CONFIG_PER_TEST)
    add_subdirectory(verification)
    add_subdirectory(std_lib)
endif()