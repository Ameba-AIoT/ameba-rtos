set(DIR ${BASEDIR}/component/application/speechmind/src)
set(
    MODULE_IFLAGS
    ${BASEDIR}/component/utils/ringbuffer
    ${BASEDIR}/component/audio/interfaces
    ${BASEDIR}/component/utils/ipc/rpc/include
    ${BASEDIR}/component/utils/ipc/rpc/src/aidl
    ${DIR}/amebalite/aidl
    ${DIR}
)

set(MODULE_C_DEFINES __RTOS__ NDEBUG)
set(MODULE_C_OPTIONS -UDEBUG -Werror)

set(
    CSRC
    ${DIR}/app_example.c
    ${DIR}/speech_handler.c
    ${DIR}/speech_player.c
    ${DIR}/speech_task.c
    ${DIR}/speech_tts.c
    ${DIR}/amebalite/speech_config.c
    ${DIR}/amebalite/speech_manager.c
    ${DIR}/amebalite/aidl/VoiceRPC_Agent_clnt.c
    ${DIR}/amebalite/aidl/VoiceRPC_Agent_svc.c
    ${DIR}/amebalite/aidl/VoiceRPC_Agent_xdr.c
    ${DIR}/amebalite/aidl/VoiceRPCBaseDS_xdr.c
    ${DIR}/amebalite/aidl/VoiceRPC_System_clnt.c
    ${DIR}/amebalite/aidl/VoiceRPC_System_svc.c
    ${DIR}/amebalite/aidl/VoiceRPC_System_xdr.c
)

ameba_internal_library(speechmind)
target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_DEFINES})
target_compile_options(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_OPTIONS})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
