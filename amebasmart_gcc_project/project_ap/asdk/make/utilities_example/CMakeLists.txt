message("EXAMPLE: " ${EXAMPLE})

#NOTICE: The source file name and folder name for 'example' should match. That means when you
#want to define a example named 'app', the directory of the source file must be component/example/.../app/example_app.c
#The build cmd is "cmake .. -DEXPAMLE=app -G Ninja && ninja" or "./build.py -a app".

if (d_ENABLE_EXAMPLE)
    file(GLOB_RECURSE EXAMPLEDIR
        ${BASEDIR}/component/example/example_${EXAMPLE}.c
        ${BASEDIR}/component/example/example_${EXAMPLE}.cc)
    cmake_path(REMOVE_FILENAME EXAMPLEDIR)
    if(EXAMPLEDIR)
        message("THE PATH of example_${EXAMPLE}.c is " "${EXAMPLEDIR}")
    else()
        message(SEND_ERROR "example_${EXAMPLE}.c not found. Please check example name!")
    endif()
    add_subdirectory(${EXAMPLEDIR} ${CMAKE_CURRENT_BINARY_DIR}/${EXAMPLE})
endif()
