set(DIR ${BASEDIR}/component/file_system/kv)

set( 
    CSRC 
    ${DIR}/kv.c
)

set( 
    MODULE_IFLAGS 
    ${BASEDIR}/component/file_system/littlefs 
    ${BASEDIR}/component/file_system/littlefs/r2.50 
)

if(CONFIG_INTERNAL_LIB)
    ameba_internal_library(kv)
    target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
    target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})

else()
    set_source_files_properties(${CSRC} DIRECTORY ../.. PROPERTIES INCLUDE_DIRECTORIES "${MODULE_IFLAGS}")

    target_sources(
        target_img2_${PROJECT_NAME}
        PRIVATE 
        ${CSRC}
    )
endif()