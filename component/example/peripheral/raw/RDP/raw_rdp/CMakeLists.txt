ameba_internal_library(example_raw_rdp)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_raw_rdp.c
    app_example.c
)

execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/copy_files/secure_src.c ${c_CMPT_IMG3_DIR})