# rename_sections.cmake
execute_process(
    COMMAND ${CMAKE_OBJDUMP} -h ${LIBRARY_LOCATION}
    OUTPUT_VARIABLE OBJDUMP_OUTPUT
)

set(OBJCOPY_COMMANDS)

list(APPEND OBJCOPY_COMMANDS  --rename-section .text=${PREPEND}.text)
string(REGEX MATCHALL "\\.text\\.[^ \t\n]+" TEXT_SECTIONS "${OBJDUMP_OUTPUT}")
foreach(SECTION ${TEXT_SECTIONS})
    list(APPEND OBJCOPY_COMMANDS  --rename-section ${SECTION}=${PREPEND}${SECTION})
endforeach()

list(APPEND OBJCOPY_COMMANDS  --rename-section .rodata=${PREPEND}.rodata)
string(REGEX MATCHALL "\\.rodata\\.[^ \t\n]+" RODATA_SECTIONS "${OBJDUMP_OUTPUT}")
foreach(SECTION ${RODATA_SECTIONS})
    list(APPEND OBJCOPY_COMMANDS  --rename-section ${SECTION}=${PREPEND}${SECTION})
endforeach()

execute_process(
    COMMAND ${CMAKE_OBJCOPY} ${OBJCOPY_COMMANDS} ${LIBRARY_LOCATION}
    RESULT_VARIABLE OBJCOPY_RESULT
)
if(NOT OBJCOPY_RESULT EQUAL 0)
    message(FATAL_ERROR "objcopy failed with exit code ${OBJCOPY_RESULT}")
endif()


