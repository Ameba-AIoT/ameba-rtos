set(DIR ${TARGETDIR}/misc)

set(
    MODULE_IFLAGS
    ${FREERTOSDIR}/include
    ${FREERTOSDIR}/portable/GCC/AmebaDplus_KM0
)

set(
    CSRC
    ${DIR}/ameba_pmu.c 
)

ameba_internal_library(misc)
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})