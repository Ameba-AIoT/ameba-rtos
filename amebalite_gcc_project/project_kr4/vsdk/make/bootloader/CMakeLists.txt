set(DIR ${TARGETDIR}/bootloader)
set(CFGDIR ${TARGETDIR}/usrcfg)

set(
    MODULE_IFLAGS
    ${DIR}/../include/
    ${FREERTOSDIR}/include/
    ${FREERTOSDIR}/portable/GCC/AmebaLite_KR4/
)

set(
    CSRC
    ${DIR}/bootloader_kr4.c
    ${CFGDIR}/ameba_bootcfg.c
)

ameba_internal_library(bootloader)
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
