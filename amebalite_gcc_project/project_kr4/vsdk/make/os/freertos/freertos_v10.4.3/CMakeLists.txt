set(DIR ${BASEDIR}/component/os/freertos/freertos_${FREERTOS_VER})
set(DIR_PORT ${DIR}/Source/portable/GCC/AmebaLite_KR4)

if(CONFIG_WIFI_EN)
    set(DIR_OSDEP ${BASEDIR}/component/os_dep)
endif()

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
    ${DIR_PORT}/rtk-port.c
    ${DIR_PORT}/portASM.S
    ${DIR}/Source/portable/MemMang/heap_5.c
)

set(
    MODULE_IFLAGS
    ${FREERTOSDIR}/include
    ${FREERTOSDIR}/portable/GCC/AmebaLite_KR4
)

target_sources(
    ${CURRENT_LIB_NAME}
    PRIVATE
    ${CSRC}
)

