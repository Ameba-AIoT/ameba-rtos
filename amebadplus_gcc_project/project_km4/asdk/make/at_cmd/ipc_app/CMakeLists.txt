set(IMQDIR ${BASEDIR}/component/soc/amebalite/app/ipc_app/src)

set(
    CSRC  
    ${IMQDIR}/imq/ipc_message_queue_api.c
    ${IMQDIR}/usrcfg/ipc_cores_cfg.c
    ${IMQDIR}/share_memory/share_memory_api.c
)

set(
    MODULE_IFLAGS 
    ${BASEDIR}/component/soc/amebalite/app/ipc_app
    ${BASEDIR}/component/soc/amebalite/app/ipc_app/include
    ${BASEDIR}/component/soc/amebalite/app/ipc_app/src
)


ameba_internal_library(ipc_app)
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})

