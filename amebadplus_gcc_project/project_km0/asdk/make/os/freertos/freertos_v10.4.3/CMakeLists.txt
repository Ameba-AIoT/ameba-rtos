set(DIR ${BASEDIR}/component/os/freertos/freertos_${FREERTOS_VER})
set(DIR_PORT ${DIR}/Source/portable/GCC/AmebaDplus_KM0)

set(
    CSRC
    ${DIR}/Source/tasks.c 
    ${DIR}/Source/list.c 
    ${DIR}/Source/croutine.c 
    ${DIR}/Source/queue.c 
    ${DIR}/Source/timers.c 
    ${DIR}/Source/event_groups.c 
    ${DIR}/Source/stream_buffer.c
    ${DIR_PORT}/port.c 
    ${DIR}/Source/portable/MemMang/heap_5.c
)

target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})