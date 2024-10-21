set(DIR ${BASEDIR}/component/audio/media_utils)

set(
    MODULE_IFLAGS
    ${BASEDIR}/component/audio/base/libs/src/include
    ${BASEDIR}/component/audio/base/log/interfaces
    ${BASEDIR}/component/audio/base/osal/osal_c/interfaces
    ${BASEDIR}/component/audio/base/osal/osal_cxx/interfaces
    ${DIR}/include
)

set(MODULE_CPP_DEFINES __RTOS__)
set(MODULE_CPP_OPTIONS -Wno-multichar -Wno-unused-parameter)

set(
    CPPSRC 
    ${DIR}/rstring.cpp
    ${DIR}/byte_utils.cpp
    ${DIR}/hexdump.cpp
    ${DIR}/rbuffer.cpp
    ${DIR}/message_handler.cpp
    ${DIR}/message_looper.cpp
    ${DIR}/message_looper_manager.cpp
    ${DIR}/message.cpp
    ${DIR}/rstate_machine.cpp
    ${DIR}/unicode.cpp
)

set_source_files_properties(${CPPSRC} DIRECTORY .. PROPERTIES COMPILE_DEFINITIONS "${MODULE_CPP_DEFINES}")
set_source_files_properties(${CPPSRC} DIRECTORY .. PROPERTIES COMPILE_OPTIONS "${MODULE_CPP_OPTIONS}")
set_source_files_properties(${CPPSRC} DIRECTORY .. PROPERTIES INCLUDE_DIRECTORIES "${MODULE_IFLAGS}")
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CPPSRC})
