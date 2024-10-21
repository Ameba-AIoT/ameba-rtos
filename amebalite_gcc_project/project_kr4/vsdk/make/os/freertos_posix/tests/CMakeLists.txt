set(DIR ${BASEDIR}/component/os/freertos/freertos_posix)
set(SOURCE_DIR ${DIR}/FreeRTOS-Plus-POSIX/test)
set(UNITY_DIR ${BASEDIR}/component/utils/unity)

set(
    MODULE_IFLAGS
    ${DIR}/include
    ${DIR}/FreeRTOS-Plus-POSIX/include
    ${DIR}/FreeRTOS-Plus-POSIX/include/portable/realtek
    ${FREERTOSDIR}/include
    ${FREERTOSDIR}/portable/GCC/AmebaLite_KR4
    ${UNITY_DIR}/src
    ${UNITY_DIR}/extras/bdd/src
    ${UNITY_DIR}/extras/memory/src
    ${UNITY_DIR}/extras/fixture/src
)

set(
    CSRC
    ${SOURCE_DIR}/iot_test_posix_clock.c
	${SOURCE_DIR}/iot_test_posix_pthread.c
	${SOURCE_DIR}/iot_test_posix_stress.c
	${SOURCE_DIR}/iot_test_posix_unistd.c
	${SOURCE_DIR}/iot_test_posix_mqueue.c
	${SOURCE_DIR}/iot_test_posix_semaphore.c
	${SOURCE_DIR}/iot_test_posix_timer.c
	${SOURCE_DIR}/iot_test_posix_utils.c
	${SOURCE_DIR}/all_tests_runner.c
)


ameba_internal_library(freertos_posix_tests)
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
