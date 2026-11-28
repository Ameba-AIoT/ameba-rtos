set(public_includes)
set(public_definitions)
set(public_libraries)

ameba_global_include(${public_includes})
ameba_global_define(${public_definitions})
ameba_global_library(${public_libraries})

set(private_sources)
set(private_includes)
set(private_definitions)
set(private_compile_options)

ameba_list_append(private_sources
    example_raw_gspi_host_ps.c
    app_example.c
)

# GSPI protocol version of the device: 1 (AmebaGreen2 device) or 2 (RLE1509 device).
set(gspi_ver 2)
# With CONFIG_WHC_INTF_GSPI the menuconfig GSPI_PROTO decides the version.
if(CONFIG_WHC_INTF_GSPI)
    set(gspi_ver ${CONFIG_GSPI_PROTO})
endif()
ameba_list_append(private_sources
    ${c_CMPT_WIFI_DIR}/whc/whc_host_rtos/gspi/rtw_gspi_drvio_v${gspi_ver}.c
)
ameba_list_append(private_definitions
    GSPI_DRV_VER=${gspi_ver}
)
ameba_list_append(private_includes
    ${c_CMPT_WIFI_DIR}/whc/whc_host_rtos/gspi
)

ameba_add_internal_library(example_raw_gspi_host_ps
    p_SOURCES
        ${private_sources}
    p_INCLUDES
        ${private_includes}
    p_DEFINITIONS
        ${private_definitions}
    p_COMPILE_OPTIONS
        ${private_compile_options}
)
