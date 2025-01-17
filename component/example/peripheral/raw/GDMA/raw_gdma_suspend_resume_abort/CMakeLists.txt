ameba_internal_library(example_raw_gdma_suspend_resume_abort)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_raw_gdma_suspend_resume_abort.c
    app_example.c
)
