if (NOT CONFIG_SPEECHMIND_EN)
    return()
endif()

ameba_add_subdirectory(src)
ameba_add_subdirectory(libs)
