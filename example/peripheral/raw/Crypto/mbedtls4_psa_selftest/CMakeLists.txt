##########################################################################################
## mbedTLS 4.1.1 PSA driver selftest example

set(public_includes)
set(public_definitions)
set(public_libraries)

ameba_global_include(${public_includes})
ameba_global_define(${public_definitions})
ameba_global_library(${public_libraries})

##########################################################################################

set(private_sources)
set(private_includes)
set(private_definitions)
set(private_compile_options)

ameba_list_append(private_includes
    ${CMAKE_CURRENT_SOURCE_DIR}
)

ameba_list_append(private_sources
    example_mbedtls4_psa_selftest.c
    app_example.c
    selftest_hash.c
    selftest_cipher.c
    selftest_aead.c
    selftest_mac.c
    selftest_ecdsa.c
    selftest_ecdh.c
)

ameba_add_internal_library(mbedtls4_psa_selftest
    p_SOURCES
        ${private_sources}
    p_INCLUDES
        ${private_includes}
    p_DEFINITIONS
        ${private_definitions}
    p_COMPILE_OPTIONS
        ${private_compile_options}
)
