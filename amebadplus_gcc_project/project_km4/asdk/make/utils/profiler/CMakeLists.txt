set(DIR ${BASEDIR}/component/utils/profiler)
set(MODULE_IFLAGS ${DIR})

set(CSRC ${DIR}/profiler.c)

if(CONFIG_INTERNAL_LIB)
    ameba_internal_library(utils_profiler)
    target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
    target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
else()
    set_source_files_properties(${CSRC} DIRECTORY ../.. PROPERTIES INCLUDE_DIRECTORIES "${MODULE_IFLAGS}")
    target_sources(target_img2_${PROJECT_NAME} PRIVATE ${CSRC})
endif()