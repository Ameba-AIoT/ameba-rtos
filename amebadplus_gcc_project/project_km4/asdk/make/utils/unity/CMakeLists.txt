set(DIR  ${BASEDIR}/component/utils/unity)
set(
    MODULE_IFLAGS 
    ${DIR}/src 
	${DIR}/extras/bdd/src 
	${DIR}/extras/memory/src 
	${DIR}/extras/fixture/src
)

set(
    CSRC 
    ${DIR}/src/unity.c 
	${DIR}/extras/memory/src/unity_memory.c 
	${DIR}/extras/fixture/src/unity_fixture.c
)

if(CONFIG_INTERNAL_LIB)
    ameba_internal_library(utils_unity)
    target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
    target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
else()
    set_source_files_properties(${CSRC} DIRECTORY ../.. PROPERTIES INCLUDE_DIRECTORIES "${MODULE_IFLAGS}")
    target_sources(target_img2_${PROJECT_NAME} PRIVATE ${CSRC})
endif()