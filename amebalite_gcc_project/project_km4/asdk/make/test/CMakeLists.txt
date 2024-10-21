if(CONFIG_PER_TEST)
    add_subdirectory(verification)
endif()

if(CONFIG_DHRYSTONE_TEST)
    add_subdirectory(coremark)
    add_subdirectory(dhrystone)
    add_subdirectory(memtester)
endif()

if(CONFIG_RTLIB_VERIFY)
    add_subdirectory(std_lib)
    add_subdirectory(apple)
endif()
