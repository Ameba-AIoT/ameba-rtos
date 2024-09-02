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

if(CONFIG_INTERNAL_LIB)

    ameba_internal_library(ipc_app)
    target_include_directories(${CURRENT_LIB_NAME} PRIVATE 4{MODULE_IFLAGS})
    target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})

else()

    set_source_files_properties(${CSRC} DIRECTORY ../.. PROPERTIES INCLUDE_DIRECTORIES "${MODULE_IFLAGS}")

    target_sources(
        target_img2_${PROJECT_NAME}
        PRIVATE
        ${CSRC}
    )
endif()
