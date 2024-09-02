set(DIR ${BASEDIR}/component/ssl/mbedtls-2.16.9)
set(DIR_RAMMAP  ${BASEDIR}/component/ssl/ssl_ram_map)

set(MODULE_C_DEFINES CONFIG_BUILD_SECURE=1)

set(
    CSRC 
    ${DIR}/library/aes.c
    ${DIR}/library/base64.c
    ${DIR}/library/oid.c
    ${DIR}/library/pem.c
    ${DIR}/library/pk.c
    ${DIR}/library/pk_wrap.c
    ${DIR}/library/platform.c
    ${DIR}/library/rsa.c
    ${DIR}/library/rsa_internal.c
    ${DIR}/library/ssl_tls.c
    ${DIR_RAMMAP}/ssl_ram_map.c		
    ${DIR}/library/pkparse.c
)

set_source_files_properties(${CSRC} DIRECTORY ../.. PROPERTIES COMPILE_DEFINITIONS "${MODULE_C_DEFINES}")

target_sources(target_img3 PRIVATE ${CSRC})