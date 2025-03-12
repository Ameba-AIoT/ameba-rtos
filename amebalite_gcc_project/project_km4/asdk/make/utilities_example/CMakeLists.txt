#eg: cmake .. –DEXAMPLE=ssl_client
message("EXAMPLE: " ${EXAMPLE})

#NOTICE: The source file name and folder name for 'example' should match. That means when you
#want to define a example named 'app', the directory of the source file must be component/example/.../app/example_app.c
#The build cmd is "cmake .. -DEXPAMLE=app -G Ninja && ninja" or "./build.py -a app".

if (EXAMPLEDIR)
    add_subdirectory(${EXAMPLEDIR} ${CMAKE_CURRENT_BINARY_DIR}/${EXAMPLE})
endif()
