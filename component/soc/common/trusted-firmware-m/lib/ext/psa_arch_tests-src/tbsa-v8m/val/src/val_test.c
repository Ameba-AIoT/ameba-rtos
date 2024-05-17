/** @file
 * Copyright (c) 2018-2019, Arm Limited or its affiliates. All rights reserved.
 * SPDX-License-Identifier : Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
**/

#include "val_infra.h"
#include "val_test.h"
#include "val_interfaces.h"
#include "val_peripherals.h"
#include "pal_interfaces.h"

/* globals */
addr_t          g_s_test_info_addr;
addr_t          g_ns_test_info_addr;
extern addr_t   g_test_binary_src_addr;
extern uint32_t g_test_binary_in_ram;
static const unsigned char elf_magic_header[ELF_IDENT] = {
    0x7f, 0x45, 0x4c, 0x46,     /* 0x7f, 'E', 'L', 'F' */
    0x01,                       /* Only 32-bit objects */
    0x01,                       /* Only LSB data. */
    0x01,                       /* Only ELF version 1. */
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0,
    0x0
};

/**
    @brief    - This function parses ELF header, entry address(test info address) and program headers.
                Copies the loadable segments to system memory.
    @param    - saddr              : Source location of tbsa_test_combined.bin
              - info_addr          : Populates the entry address from S/NS test ELF
              - test_binary_in_ram : 1 - if present in RAM, 0 - otherwise
    @return   - tbsa_status_t
**/
static tbsa_status_t val_test_elf_load(addr_t saddr, addr_t *info_addr, uint32_t test_binary_in_ram)
{
    tbsa_elf_header_t test_elfh;
    tbsa_pheader_t    test_ph;
    int               i;

    if (test_binary_in_ram == 1UL) {
        memcpy(&test_elfh, (void*)saddr, sizeof(tbsa_elf_header_t));
    } else {
        if (0 != pal_spi_read (saddr, (uint8_t *)&test_elfh, sizeof(tbsa_elf_header_t))) {
            val_print(PRINT_ERROR, "Error: SPI read failure for Test ELF header\n", 0);
            return TBSA_STATUS_LOAD_ERROR;
        }
    }
    /* validate ELF header */
    if (0 != memcmp(&test_elfh.e_ident, &elf_magic_header, ELF_IDENT)) {
        val_print(PRINT_ERROR, "Fail: Test ELF header validation\n", 0);
        return TBSA_STATUS_INCORRECT_VALUE;
    }

    for(i=0; i < test_elfh.e_phnum; i++) {
        /* Read the program header */
        if (test_binary_in_ram == 1UL) {
            memcpy(&test_ph, (const void*)(saddr + test_elfh.e_phoff + (sizeof(tbsa_pheader_t)*i)), sizeof(tbsa_pheader_t));
        } else {
            if (0 != pal_spi_read ((saddr + test_elfh.e_phoff + (sizeof(tbsa_pheader_t)*i)), (uint8_t *)&test_ph, sizeof(tbsa_pheader_t))) {
                val_print(PRINT_ERROR, "Error: reading Test program header\n", 0);
                return TBSA_STATUS_LOAD_ERROR;
            }
        }
        /* Load the program to physical RAM */
        if (test_binary_in_ram == 1UL) {
            memcpy((void*)test_ph.p_paddr, (const void*)(saddr + test_ph.p_offset), test_ph.p_filesz);
        } else {
            if (0 != pal_spi_read ((saddr + test_ph.p_offset), (uint8_t *)test_ph.p_paddr, test_ph.p_filesz)) {
                val_print(PRINT_ERROR, "Error: loading Test program\n", 0);
                return TBSA_STATUS_LOAD_ERROR;
            }
        }
    }

    *info_addr = (addr_t)test_elfh.e_entry;

    return TBSA_STATUS_SUCCESS;
}

/**
    @brief        - This function reads the test ELFs from RAM or secondary storage and loads into
                    system memory
    @param        - test_id      : Returns the current test ID
                  - test_id_prev : Previous test ID.
    @return       - Updates the test_id
**/
tbsa_status_t val_test_load(test_id_t *test_id, test_id_t test_id_prev)
{
    tbsa_test_header_t test_header;
    addr_t             sflash_addr = g_test_binary_src_addr;

    /*
       The combined Test ELF binary:
       +---------------+             +----------------+
       | TEST#0 header |-----------> |  START MARKER  |
       |               |             |----------------|
       | Test#0 S ELF  |             |   Test#n ID    |
       |               |             |----------------|
       | Test#0 NS ELF |             | Test#n S size  |
       +---------------+             |----------------|
       | TEST#1 header |             | Test#n NS size |
       |               |             +----------------+
       | Test#1 S ELF  |
       |               |
       | Test#1 NS ELF |
       +---------------+
              |
              .
               |
               .
       +---------------+
       | TEST#n header |
       |               |
       | Test#n S ELF  |
       |               |
       | Test#n NS ELF |
       +---------------+
       |  END MARKER   |
       +---------------+
     */

    if (test_id_prev != TBSA_TEST_INVALID) {
        /* Reading TBSA header */
        do
        {
            if (val_spi_read(sflash_addr, (uint8_t *)&test_header, sizeof(tbsa_test_header_t))) {
                val_print(PRINT_INFO, "\n\n\rError: reading custom Test header", 0);
                return TBSA_STATUS_LOAD_ERROR;
            }

            if (test_header.start_marker == TBSA_TEST_END_MARKER) {
                val_print(PRINT_ERROR, "\n\n\rNo more valid tests found. Exiting.", 0);
                *test_id = TBSA_TEST_INVALID;
                return TBSA_STATUS_SUCCESS;
            }

            if (test_header.start_marker != TBSA_TEST_START_MARKER) {
                sflash_addr += 0x4;
                continue;
            }

            if ((test_header.start_marker == TBSA_TEST_START_MARKER) && (test_header.test_id == test_id_prev)) {
                sflash_addr += (sizeof(tbsa_test_header_t) + test_header.s_elf_size + test_header.ns_elf_size);
                break;
            }

            sflash_addr += (sizeof(tbsa_test_header_t) + test_header.s_elf_size + test_header.ns_elf_size);
        }while(1);
    }

    if (val_spi_read(sflash_addr, (uint8_t *)&test_header, sizeof(tbsa_test_header_t))) {
        val_print(PRINT_INFO, "\n\n\rError: reading custom Test header", 0);
        return TBSA_STATUS_LOAD_ERROR;
    }

    if (test_header.start_marker == TBSA_TEST_END_MARKER) {
        val_print(PRINT_ERROR, "\n\n\rNo more valid tests found. Exiting.", 0);
        *test_id = TBSA_TEST_INVALID;
        return TBSA_STATUS_SUCCESS;
    }

    if (test_header.start_marker != TBSA_TEST_START_MARKER) {
        val_print(PRINT_ERROR, "\n\n\rNo valid test binary found. Exiting.", 0);
        *test_id = TBSA_TEST_INVALID;
        return TBSA_STATUS_LOAD_ERROR;
    }

    /* load S image */
    sflash_addr += sizeof(tbsa_test_header_t);
    if (val_test_elf_load(sflash_addr, &g_s_test_info_addr, g_test_binary_in_ram)) {
        val_print(PRINT_INFO, "\n\rError: loading Test ELF", 0);
        return TBSA_STATUS_LOAD_ERROR;
    }

    /* load NS image */
    sflash_addr += test_header.s_elf_size;
    if (val_test_elf_load(sflash_addr, &g_ns_test_info_addr, g_test_binary_in_ram)) {
        val_print(PRINT_INFO, "\n\rError: loading Test ELF", 0);
        return TBSA_STATUS_LOAD_ERROR;
    }

    *test_id = test_header.test_id;

    return TBSA_STATUS_SUCCESS;
}

/**
    @brief    - This function reads the function pointer addresses for
                entry/payload/exit test functions
    @param    - test_id  : Current test ID
              - info_type: Test function type
              - paddr    : Returns the Test function address
    @return   - Returns tbsa_status_t
**/
tbsa_status_t val_test_get_info(test_id_t test_id, tbsa_test_fn_type_t info_type, addr_t *paddr)
{
    switch (info_type)
    {
        case ENTRY_FUNC_HOOK_S:
            *paddr = (addr_t)(((tbsa_test_info_t *)g_s_test_info_addr)->test_fun[0]);
            break;

        case PAYLOAD_FUNC_S:
            *paddr = (addr_t)(((tbsa_test_info_t *)g_s_test_info_addr)->test_fun[1]);
            break;

        case EXIT_FUNC_HOOK_S:
            *paddr = (addr_t)(((tbsa_test_info_t *)g_s_test_info_addr)->test_fun[2]);
            break;

        case ENTRY_FUNC_HOOK_NS:
            *paddr = (addr_t)(((tbsa_test_info_t *)g_ns_test_info_addr)->test_fun[0]);
            break;

        case PAYLOAD_FUNC_NS:
            *paddr = (addr_t)(((tbsa_test_info_t *)g_ns_test_info_addr)->test_fun[1]);
            break;

        case EXIT_FUNC_HOOK_NS:
            *paddr = (addr_t)(((tbsa_test_info_t *)g_ns_test_info_addr)->test_fun[2]);
            break;
    }
    return TBSA_STATUS_SUCCESS;
}

/**
    @brief    - Initialize bss for a given test
    @param    - init     : Start and end labels for bss section
    @return   - tbsa_status_t
**/
tbsa_status_t val_test_initialize(tbsa_test_init_t *init)
{
    /* zero bss section */
    memset(init->bss_start, 0, TBSA_REGION_SIZE(init->bss_start, init->bss_end));

    return TBSA_STATUS_SUCCESS;
}
