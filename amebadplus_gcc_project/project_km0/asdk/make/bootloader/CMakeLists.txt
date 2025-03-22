set(DIR  ${TARGETDIR}/bootloader)
set(CFGDIR  ${BASEDIR}/component/soc/usrcfg/${c_SOC_TYPE})
set(DIR_COM  ${TARGETDIR}/fwlib/ram_common)
set(DIR_RAM_LP  ${TARGETDIR}/fwlib/ram_km0)
set(MODULE_IFLAGS ${DIR}/../include)

set(
    CSRC 
    ${DIR}/bootloader_km0.c
    ${CFGDIR}/ameba_bootcfg.c
)

ameba_internal_library(bootloader)

target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})