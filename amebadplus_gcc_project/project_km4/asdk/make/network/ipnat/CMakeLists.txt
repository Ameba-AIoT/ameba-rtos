set(DIR ${BASEDIR}/component/lwip/lwip_${LWIP_VER}/src/core/ipv4/ip_nat)
set(MODULE_IFLAGS ${BASEDIR}/component/lwip/lwip_${LWIP_VER}/src/core/ipv4/ip_nat)

set(
    CSRC 
    ${DIR}/lwip_ip4nat.c
)

set(
    MODULE_C_DEFINES 
    IP_FORWARD
    IP_NAT
)

ameba_app_library(ipnat)
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_DEFINES})