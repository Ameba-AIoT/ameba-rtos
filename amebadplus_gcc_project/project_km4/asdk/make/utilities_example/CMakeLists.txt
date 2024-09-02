#eg: cmake .. –DEXAMPLE=ssl_client 
message("EXAMPLE: " ${EXAMPLE})
if (EXAMPLE)
    file(GLOB_RECURSE EXAMPLEDIR ${BASEDIR}/component/example/example_${EXAMPLE}.h)
    cmake_path(REMOVE_FILENAME EXAMPLEDIR)
    message("THE PATH of example_${EXAMPLE}.h is " "${EXAMPLEDIR}")
    file(GLOB EXAMPLE_SRC ${EXAMPLEDIR}*.c)
endif()
unset(EXAMPLE CACHE)


target_sources(target_img2_${PROJECT_NAME} PRIVATE ${EXAMPLE_SRC})