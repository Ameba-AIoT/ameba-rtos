#-------------------------------------------------------------------------------
# Copyright (c) 2022, Arm Limited. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.13)

########################## TEST Configuration ##################################

include(utils)
    dump_options("TEST Configuration"
    "
        TEST_NS;
        TEST_S;
        TEST_NS_ATTESTATION;
        TEST_NS_CRYPTO;
        TEST_NS_ITS;
        TEST_NS_PS;
        TEST_NS_QCBOR;
        TEST_NS_T_COSE;
        TEST_NS_PLATFORM;
        TEST_NS_FWU;
        TEST_NS_IPC;
        TEST_NS_SLIH_IRQ;
        TEST_NS_FLIH_IRQ;
        TEST_NS_MULTI_CORE;
        TEST_NS_MANAGE_NSID;
        TEST_NS_SFN_BACKEND;
        TEST_NS_FPU;
        TEST_S_ATTESTATION;
        TEST_S_CRYPTO;
        TEST_S_ITS;
        TEST_S_PS;
        TEST_S_PLATFORM;
        TEST_S_FWU;
        TEST_S_IPC;
        TEST_S_SFN_BACKEND;
        TEST_S_FPU;
    "
    )

include(secure_tests.cmake)

# Include test suites at last after other targets are setup.
add_subdirectory(suites)
