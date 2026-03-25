set(
    MODULE_IFLAGS
    ${BASEDIR}/component/utils/ringbuffer
)
ameba_internal_library(example_ota_user)
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_ota_user.c
    app_example.c
)
