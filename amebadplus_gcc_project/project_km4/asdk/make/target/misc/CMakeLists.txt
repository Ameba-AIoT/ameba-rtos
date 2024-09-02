set(DIR ${TARGETDIR}/misc)

set(
    MODULE_IFLAGS
    ${FREERTOSDIR}/include/
    ${FREERTOSDIR}/portable/GCC/AmebaDplus_KM4/non_secure
    ${FREERTOSDIR}/portable/GCC/AmebaDplus_KM4/secure
)

set(CSRC ${DIR}/ameba_pmu.c)

if(CONFIG_AS_INIC_AP OR CONFIG_SINGLE_CORE_WIFI)
    list(
        APPEND CSRC
        ${DIR}/ameba_ota.c
    )
endif()

if(CONFIG_INTERNAL_LIB)
    ameba_internal_library(target_misc)
    target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
    target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
else()
    set_source_files_properties(${CSRC} DIRECTORY ../.. PROPERTIES INCLUDE_DIRECTORIES "${MODULE_IFLAGS}")
    target_sources(target_img2_${PROJECT_NAME} PRIVATE ${CSRC})
endif()
