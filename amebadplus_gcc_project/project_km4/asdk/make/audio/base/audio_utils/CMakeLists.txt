set(DIR ${BASEDIR}/component/audio/base/audio_utils)

set(MODULE_IFLAGS 
    ${BASEDIR}/component/audio/base/log/interfaces
    ${DIR}/include
)

set(
    CSRC 
    ${DIR}/src/format.c
    ${DIR}/src/primitives.c
)


set_source_files_properties(${CSRC} DIRECTORY .. PROPERTIES INCLUDE_DIRECTORIES "${MODULE_IFLAGS}")
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})