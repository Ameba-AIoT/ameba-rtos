set(DIR ${BASEDIR}/component/audio/audio_algorithm/sonic)

set(MODULE_C_OPTIONS -D__RTOS__ -Wno-multichar -Wno-unused-parameter -Werror)

set(CSRC ${DIR}/sonic.c)

set_source_files_properties(${CSRC} DIRECTORY ../.. PROPERTIES COMPILE_OPTIONS "${MODULE_C_OPTIONS}")
target_sources(audio_mixer_${PROJECT_NAME} PRIVATE ${CSRC})
