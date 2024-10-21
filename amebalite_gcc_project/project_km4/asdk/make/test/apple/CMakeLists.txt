set(DIR ${TARGETDIR}/verification/swlib/apple)

set(
    MODULE_IFLAGS
    ${BASEDIR}/component/application/apple/homekit/crypto/chacha 
	${BASEDIR}/component/application/apple/homekit/crypto/ed25519 
	${BASEDIR}/component/application/apple/homekit/crypto/ed25519/core 
	${BASEDIR}/component/application/apple/homekit/crypto/poly1305 
	${BASEDIR}/component/application/apple/homekit/crypto/sha512 
	${BASEDIR}/component/application/apple/WACServer/External/Curve25519 
	${BASEDIR}/component/application/apple/WACServer/External/GladmanAES 
)

set(
    CSRC 
    ${DIR}/curve25519-donna-test.c
    ${DIR}/ed25519_test.c 
    ${DIR}/sha512_hkdf_test.c
    ${DIR}/chachatest.c 
    ${DIR}/poly1305test.c 
)


ameba_internal_library(test_apple)
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
