if (__INCLUDED_UTILS_BUILD_CMAKE__)
    return()
else()
    set(__INCLUDED_UTILS_BUILD_CMAKE__ TRUE)
endif()

add_library(src_${d_MCU_PROJECT_NAME}_utils_ipc INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_utils_rpc_framework_aidl INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_utils_rpc_framework_libs INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_utils_rpc_framework_src INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_utils_rpc_framework_xip INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_utils_rpc_hal INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_utils_rpc_test INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_utils_ringbuffer INTERFACE)

#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_utils_ipc p_SCOPE interface
    ${c_COMPONENT_DIR}/utils/ipc/ipc/${d_PLATFORM_TYPE}/src/ipc.c
)
ameba_target_sources_if(CONFIG_IMQ2_TEST src_${d_MCU_PROJECT_NAME}_utils_ipc p_SCOPE interface
    ${c_COMPONENT_DIR}/utils/ipc/ipc/${d_PLATFORM_TYPE}/tests/test_ipc.c
)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_utils_rpc_framework_aidl p_SCOPE interface
    ${c_COMPONENT_DIR}/utils/ipc/rpc/src/aidl/RPCBaseDS_xdr.c
)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_utils_rpc_framework_aidl p_SCOPE interface __RTOS__)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_utils_rpc_framework_libs p_SCOPE interface
	${c_COMPONENT_DIR}/utils/ipc/rpc/src/rpc_client.c
	${c_COMPONENT_DIR}/utils/ipc/rpc/src/rpc_server.c
	${c_COMPONENT_DIR}/utils/ipc/rpc/src/rpc_common.c
	${c_COMPONENT_DIR}/utils/ipc/rpc/src/rpc_utils.c
	${c_COMPONENT_DIR}/utils/ipc/rpc/src/xdr/xdr_array.c
	${c_COMPONENT_DIR}/utils/ipc/rpc/src/xdr/xdr.c
	${c_COMPONENT_DIR}/utils/ipc/rpc/src/xdr/xdr_float.c
	${c_COMPONENT_DIR}/utils/ipc/rpc/src/xdr/xdr_mem.c
	${c_COMPONENT_DIR}/utils/ipc/rpc/src/xdr/xdr_process.c
	${c_COMPONENT_DIR}/utils/ipc/rpc/src/xdr/xdr_ref.c
	${c_COMPONENT_DIR}/utils/ipc/rpc/src/xdr/xdr_sizeof.c
)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_utils_rpc_framework_libs p_SCOPE interface -Wimplicit-function-declaration)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_utils_rpc_framework_libs p_SCOPE interface __RTOS__)

#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_utils_rpc_framework_src p_SCOPE interface
	${c_COMPONENT_DIR}/utils/ipc/rpc/src/rpc_common.c
)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_utils_rpc_framework_src p_SCOPE interface -Wimplicit-function-declaration)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_utils_rpc_framework_src p_SCOPE interface __RTOS__)

#--------------------------#
ameba_target_include(src_${d_MCU_PROJECT_NAME}_utils_ringbuffer p_SCOPE interface
    ${c_COMPONENT_DIR}/utils/ringbuffer
    ${c_COMPONENT_DIR}/audio/base/log/interfaces
)
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_utils_ringbuffer p_SCOPE interface
    ${c_COMPONENT_DIR}/utils/ringbuffer/ringbuffer.c
)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_utils_ringbuffer p_SCOPE interface __RTOS__)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_utils_rpc_hal p_SCOPE interface
    ${c_COMPONENT_DIR}/utils/ipc/rpc/hal/rpc_hw_manager.c
)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_utils_rpc_hal p_SCOPE interface -UDEBUG -Wno-multichar)
ameba_target_definitions(src_${d_MCU_PROJECT_NAME}_utils_rpc_hal p_SCOPE interface __RTOS__ NDEBUG)
#--------------------------#
ameba_target_include(src_${d_MCU_PROJECT_NAME}_utils_rpc_test p_SCOPE interface
    ${c_COMPONENT_DIR}/audio/base/log/interfaces
    ${c_COMPONENT_DIR}/audio/base/osal/osal_c/interfaces
    ${c_COMPONENT_DIR}/audio/base/libs/src/include
    ${c_COMPONENT_DIR}/utils/ipc/rpc/include/xdr
    ${c_COMPONENT_DIR}/utils/ipc/rpc/src/aidl
    ${c_COMPONENT_DIR}/utils/ipc/rpc/tests/aidl
)
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_utils_rpc_test p_SCOPE interface
    ${c_COMPONENT_DIR}/utils/ipc/rpc/tests/rpc_test.c
    ${c_COMPONENT_DIR}/utils/ipc/rpc/tests/test_rpc_ap.c
    ${c_COMPONENT_DIR}/utils/ipc/rpc/tests/test_rpc_np.c
    ${c_COMPONENT_DIR}/utils/ipc/rpc/tests/test_rpc_np_client1.c
    ${c_COMPONENT_DIR}/utils/ipc/rpc/tests/test_rpc_np_client2.c

    ${c_COMPONENT_DIR}/utils/ipc/rpc/tests/aidl/Test1RPC_Agent_svc.c
    ${c_COMPONENT_DIR}/utils/ipc/rpc/tests/aidl/Test1RPC_Agent_xdr.c
    ${c_COMPONENT_DIR}/utils/ipc/rpc/tests/aidl/Test1RPCBaseDS_xdr.c
    ${c_COMPONENT_DIR}/utils/ipc/rpc/tests/aidl/Test1RPC_System_clnt.c
    ${c_COMPONENT_DIR}/utils/ipc/rpc/tests/aidl/Test1RPC_System_svc.c
    ${c_COMPONENT_DIR}/utils/ipc/rpc/tests/aidl/Test1RPC_System_xdr.c

    ${c_COMPONENT_DIR}/utils/ipc/rpc/tests/aidl/Test2RPC_Agent_svc.c
    ${c_COMPONENT_DIR}/utils/ipc/rpc/tests/aidl/Test2RPC_Agent_xdr.c
    ${c_COMPONENT_DIR}/utils/ipc/rpc/tests/aidl/Test2RPCBaseDS_xdr.c
    ${c_COMPONENT_DIR}/utils/ipc/rpc/tests/aidl/Test2RPC_System_clnt.c
    ${c_COMPONENT_DIR}/utils/ipc/rpc/tests/aidl/Test2RPC_System_svc.c
    ${c_COMPONENT_DIR}/utils/ipc/rpc/tests/aidl/Test2RPC_System_xdr.c
)
#--------------------------#