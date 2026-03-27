set(BASEDIR ${c_BASEDIR})

if (c_ENABLE_EXAMPLE)
    if (EXAMPLEDIR)
        message("EXAMPLEDIR = ${EXAMPLEDIR}")
        ameba_add_subdirectory(${EXAMPLEDIR})
    endif()
endif()
