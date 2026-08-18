##########################################################################################
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
    ${c_CMPT_IMG3_DIR}/include
    # pk_wrap.h (internal mbedtls header) needed for custom mbedtls_pk_info_t
    # that routes ECDSA signing to Secure World via NSC.
    ${c_CMPT_SSL_DIR}/mbedtls-${v_MBEDTLS_VER}/library
)

ameba_list_append(private_sources
    example_rdp_mutual_auth_ecdsa.c
    app_example.c
)

ameba_add_internal_library(example_rdp_mutual_auth_ecdsa
    p_SOURCES
        ${private_sources}
    p_INCLUDES
        ${private_includes}
    p_DEFINITIONS
        ${private_definitions}
    p_COMPILE_OPTIONS
        ${private_compile_options}
)
##########################################################################################
