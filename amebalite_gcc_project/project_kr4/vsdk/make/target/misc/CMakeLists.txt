set(DIR ${TARGETDIR}/misc)

set(
    MODULE_IFLAGS
    ${FREERTOSDIR}/include/
    ${FREERTOSDIR}/portable/GCC/AmebaLite_KR4
)

set(CSRC ${DIR}/ameba_pmu.c)

if(CONFIG_AS_INIC_AP OR CONFIG_SINGLE_CORE_WIFI)
    list(
        APPEND CSRC
        ${DIR}/ameba_ota.c
    )
endif()


ameba_internal_library(target_misc)
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
