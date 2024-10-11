set(DIR ${BASEDIR}/component/ssl/mbedtls-${MBEDTLS_VER})

set(MODULE_C_DEFINES CONFIG_BUILD_SECURE=1)

set(
    CSRC 
    ${DIR}/library_nsc/mbedtls_nsc.c
)

set_source_files_properties(${CSRC} DIRECTORY ../.. PROPERTIES COMPILE_DEFINITIONS "${MODULE_C_DEFINES}")

target_sources(target_img3 PRIVATE ${CSRC})