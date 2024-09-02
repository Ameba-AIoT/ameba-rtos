set(DIR ${TARGETDIR}/swlib/libnosys)

set( 
    CSRC 
    ${DIR}/_exit.c
    ${DIR}/chown.c
    ${DIR}/close.c
    ${DIR}/environ.c
    ${DIR}/errno.c
    ${DIR}/execve.c
    ${DIR}/fork.c
    ${DIR}/fstat.c
    ${DIR}/getpid.c
    ${DIR}/gettod.c
    ${DIR}/isatty.c
    ${DIR}/kill.c
    ${DIR}/link.c
    ${DIR}/lseek.c
    ${DIR}/open.c
    ${DIR}/readlink.c
    ${DIR}/sbrk.c
    ${DIR}/stat.c
    ${DIR}/symlink.c
    ${DIR}/times.c
    ${DIR}/unlink.c
    ${DIR}/wait.c
)

set(MODULE_C_OPTIONS -Wno-unused-parameter)

if(CONFIG_INTERNAL_LIB)

    # add_compile_options(${MODULE_C_OPTIONS})
    ameba_internal_library(libnosys)
    target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
    target_compile_options(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_OPTIONS})

else()
    set_source_files_properties(${CSRC} DIRECTORY .. PROPERTIES COMPILE_OPTIONS "${MODULE_C_OPTIONS}")

    target_sources(
        target_img2_${PROJECT_NAME}
        PRIVATE 
        ${CSRC}
    )
endif()