set(DIR ${TARGETDIR}/sboot/test)

set(
    CSRC 
    ${DIR}/curve25519-donna-test.c 
    ${DIR}/ed25519_test.c 
    ${DIR}/sha512_hkdf_test.c 
    ${DIR}/ed25519_ge_fe_ram.c 
    ${DIR}/sboot_ed25519_keypair_ram.c 
    ${DIR}/sboot_ed25519_ram.c 
    ${DIR}/sboot_ed25519_sign_ram.c 
)

target_sources(target_img2_${PROJECT_NAME} PRIVATE ${CSRC})