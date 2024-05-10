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

#include "val_pe.h"
#include "val_test.h"
#include "val_infra.h"
#include "val_target.h"
#include "val_interrupt.h"
#include "val_test_common.h"
#include "pal_interfaces.h"

/* globals */
print_verbosity_t    g_print_level = PRINT_TEST;
addr_t               g_test_binary_src_addr;
uint32_t             g_test_binary_in_ram;
tbsa_status_buffer_t g_test_status_buffer[TBSA_TOTAL_TESTS];
bool_t               g_vtor_relocated_from_rom;
addr_t               g_stdio_uart_base_addr = NULL;

/* externs*/
extern tbsa_isr_vector      g_tbsa_s_isr_vector;
extern tbsa_isr_vector      g_tbsa_ns_isr_vector;
extern const tbsa_val_api_t tbsa_val_ns_api;

/**
    @brief    - Initialise test status buffer
    @return   - tbsa_status_t
**/

static tbsa_status_t val_status_buffer_init(tbsa_status_buffer_t *psbuf)
{
    int i;

    for(i=0; i < TBSA_TEST_STATUS_BUF_SIZE;)
    {
        psbuf->state   = 0x0;
        psbuf->status  = (uint8_t)TBSA_STATUS_INVALID;
        i             += sizeof(tbsa_status_buffer_t);
        psbuf++;
    }
    return TBSA_STATUS_SUCCESS;
}

/**
    @brief    - This function parses the input string and writes bytes into UART TX FIFO
    @param    - str     : Buffer of the string to print
                data    : Data value to print
    @return   - void
**/
void val_print_raw(char *str, uint32_t data)
{
    uint8_t buffer[16];
    uint8_t j;
    int8_t  i      = 0;
    char    char_0 = 48;

    for(;*str != '\0'; ++str) {
        if(*str == '%') {
            ++str;
            if(*str == 'd') {
                while(data != 0) {
                    j         = data % 10;
                    data      = data /10;
                    buffer[i] = j + 48;
                    i        += 1;
                }
            } else if(*str == 'x' || *str == 'X') {
                while(data != 0) {
                    j         = data & 0xf;
                    data    >>= 4;
                    buffer[i] = j + ((j > 9) ? 55 : 48);
                    i        += 1;
                }
            }
            if (i > 0) {
                while(i > 0) {
                    val_uart_tx(g_stdio_uart_base_addr, (void*)&buffer[--i], sizeof(uint8_t));
                }
            } else {
                val_uart_tx(g_stdio_uart_base_addr, (void*)&char_0, sizeof(uint8_t));
            }
        } else {
            val_uart_tx(g_stdio_uart_base_addr, (void*)str, sizeof(uint8_t));
        }
    }
}

/**
    @brief    - Print module
    @param    - verbosity: Print verbosity level
              - string   : Input string
              - data     : Value for format specifier
    @return   - tbsa_status_t
**/
tbsa_status_t val_print(print_verbosity_t verbosity, char *string, uint32_t data)
{
    if(verbosity >= g_print_level) {
        val_print_raw(string, data);
    }
    return TBSA_STATUS_SUCCESS;
}

/**
    @brief    - Returns the base address of target configuration block database.
    @param    - blob     : Populates the base address
    @return   - tbsa_status_t
**/
tbsa_status_t val_target_cfg_get_next(void **blob)
{
    tbsa_status_t status = TBSA_STATUS_SUCCESS;
    target_cfg_hdr_t *hdr;
    uint32_t size;

    if (*blob == NULL) {
        *blob = pal_get_target_cfg_start();
        if (blob == NULL) {
            return TBSA_STATUS_NOT_FOUND;
        }
        hdr = *blob;

        /* Sanity check signature and version here */
       if ((hdr->version != 1) || (hdr->size == 0)) {
            val_print(PRINT_ERROR, "Target config database Error. \n", 0);
            return status;
        }
        hdr++;
        *blob = hdr;  // skip the header. start with the first record.
        return status;
    }

    size = (((cfg_type_t *)*blob)->size) & 0xFFFFFF;
    if (size) {
        *blob = (void *)((uint8_t *)*blob + size);
        return TBSA_STATUS_SUCCESS;
    }
    return TBSA_STATUS_ERROR;
}

/**
    @brief    - This function checks for the given configuration ID with the block in
                target configuration database.
    @param    - cfg_id   : Configuration ID of a block
              - data     : Returns block base address
              - size     : Block size
    @return   - tbsa_status_t
**/
tbsa_status_t val_target_get_cfg_blob(cfg_id_t cfg_id, uint8_t **data, uint32_t *size)
{
    tbsa_status_t status;
    void *config_blob = NULL;

    val_print(PRINT_DEBUG, "Input id is %x \n", cfg_id);
    do {

        status = val_target_cfg_get_next(&config_blob);

        if (TBSA_ERROR(status)) {
            break;
        }

        if (((cfg_type_t *)config_blob)->cfg_id == cfg_id) {
            *data  = (uint8_t *)config_blob;
            status = TBSA_STATUS_SUCCESS;
            break;
        } else if (((((cfg_type_t *)config_blob)->cfg_id & TBSA_TEST_MAJOR_GROUP_MASK) == (cfg_id & TBSA_TEST_MAJOR_GROUP_MASK)) && \
                   !(((cfg_type_t *)config_blob)->cfg_id & (TBSA_TEST_MINOR_GROUP_MASK | TBSA_TEST_CFG_INSTANCE_MASK))) {
                config_blob = (void *)((uint8_t *)config_blob + sizeof(memory_hdr_t));
                if (((cfg_type_t *)config_blob)->cfg_id == cfg_id) {
                    *data  = (uint8_t *)config_blob;
                    status = TBSA_STATUS_SUCCESS;
                    break;
                }
        } else if (((cfg_type_t *)config_blob)->cfg_id == TBSA_TEST_INVALID_CFG_ID) {
                status = TBSA_STATUS_NOT_FOUND;
                break;
        }
    } while(1);

    return status;
}

/**

  @brief  - This function returns the data associated with a given
           config ID.
  @param  -  size  : if the input size is less than the data size to
           returned, the size is updated with the actual data size and
           error is returned.
  @return - data contains the information of type specific to a
            config id.
**/
tbsa_status_t val_target_get_config(cfg_id_t cfg_id, uint8_t **data, uint32_t *size)
{
    tbsa_status_t status;

    if ((cfg_id < TARGET_MIN_CFG_ID) || (cfg_id > TARGET_MAX_CFG_ID)) {
        val_print(PRINT_ERROR, "Invalid Target data config ID = %x \n", cfg_id);
        return TBSA_STATUS_INVALID_ARGS;
    }
    status = val_target_get_cfg_blob(cfg_id, data, size);

    if (TBSA_ERROR(status)) {
        val_print(PRINT_DEBUG, "\n\tGet Config failed with status = %x", status);
        val_print(PRINT_DEBUG, " for cfg_id = %x", cfg_id);
        return status;
    }
    return TBSA_STATUS_SUCCESS;
}

/**
    @brief    - Initialise VAL memory blocks
**/
tbsa_status_t val_memory_init (void)
{
    return val_status_buffer_init(&g_test_status_buffer[0]);
}

/**
    @brief    - Initialize NVRAM
    @param    - void
    @return   - void
**/
tbsa_status_t val_nvram_init (void)
{
    tbsa_status_t  status       = TBSA_STATUS_SUCCESS;
    memory_desc_t  *memory_desc;
    test_id_t      test_id;
    uint32_t       active_test;
    boot_t         boot;

    status = val_target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MEMORY, MEMORY_NVRAM, 0),
                                   (uint8_t **)&memory_desc,
                                   (uint32_t *)sizeof(memory_desc_t));
    if(status != TBSA_STATUS_SUCCESS) {
        return status;
    }

    status = val_nvram_read(memory_desc->start, TBSA_NVRAM_OFFSET(NV_BOOT), &boot, sizeof(boot_t));
    if(status != TBSA_STATUS_SUCCESS) {
        return status;
    }

    if (val_system_reset_type(COLD_RESET) && \
        (boot.cb != COLD_BOOT_REQUESTED) &&  \
        (boot.wb != WARM_BOOT_REQUESTED) &&  \
        (boot.wdogb != WDOG_BOOT_REQUESTED)) {
        test_id = TBSA_TEST_INVALID;
        status = val_nvram_write(memory_desc->start, TBSA_NVRAM_OFFSET(NV_TEST), &test_id, sizeof(test_id_t));
        if(status != TBSA_STATUS_SUCCESS) {
            return status;
        }
        boot.wb    = BOOT_UNKNOWN;
        boot.cb    = BOOT_UNKNOWN;
        boot.wdogb = BOOT_UNKNOWN;
        status = val_nvram_write(memory_desc->start, TBSA_NVRAM_OFFSET(NV_BOOT), &boot, sizeof(boot_t));
        if(status != TBSA_STATUS_SUCCESS) {
            return status;
        }

        active_test = 0xFFFFFFFFUL;
        status = val_nvram_write(memory_desc->start, TBSA_NVRAM_OFFSET(NV_ACT_TST), &active_test, sizeof(active_test));
        if(status != TBSA_STATUS_SUCCESS) {
            return status;
        }
    }

    return status;
}

/**
    @brief    - This function returns the test ID of the last test that was run
    @param    - void
    @return   - test_id_t
**/
test_id_t val_nvram_get_last_id (void)
{
    tbsa_status_t  status;
    memory_desc_t  *memory_desc;
    boot_t         boot;
    test_id_t      test_id;
    test_count_t   test_count;

    status = val_target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MEMORY, MEMORY_NVRAM, 0),
                                   (uint8_t **)&memory_desc,
                                   (uint32_t *)sizeof(memory_desc_t));
    if(status != TBSA_STATUS_SUCCESS) {
        val_print(PRINT_ERROR, "\n\tNVRAM not present", 0);
        return TBSA_TEST_INVALID;
    }

    status = val_nvram_read(memory_desc->start, TBSA_NVRAM_OFFSET(NV_BOOT), &boot, sizeof(boot_t));
    if(status != TBSA_STATUS_SUCCESS) {
        val_print(PRINT_ERROR, "\n\tNVRAM read error", 0);
        return TBSA_TEST_INVALID;
    }

    if ((boot.wb == WARM_BOOT_REQUESTED) || (boot.cb == COLD_BOOT_REQUESTED) || (boot.wdogb == WDOG_BOOT_REQUESTED)) {
        status = val_nvram_read(memory_desc->start, TBSA_NVRAM_OFFSET(NV_TEST), &test_id, sizeof(test_id_t));
        if(status != TBSA_STATUS_SUCCESS) {
            val_print(PRINT_ERROR, "\n\tNVRAM read error", 0);
            return TBSA_TEST_INVALID;
        }
    } else {
        memset((void*)&test_count, 0UL, sizeof(test_count_t));
        val_nvram_write(memory_desc->start, TBSA_NVRAM_OFFSET(NV_TEST_CNT), &test_count, sizeof(test_count_t));
        return TBSA_TEST_INVALID;
    }

    return test_id;
}

/**
    @brief    - Initialise VAL infrastructure
    @param    - test_id  : Populate test ID
    @return   - tbsa_status_t
**/
tbsa_status_t val_infra_init(test_id_t *test_id)
{
    tbsa_status_t         status;
    memory_desc_t         *bootrom_desc;
    soc_peripheral_desc_t *uart_desc;
    uint32_t              vtor;
    uint32_t              instance = 0;

    val_mem_reg_write(SYST_CSR, 0x0);
    val_mem_reg_write(SYST_CSR_NS, 0x0);

    do{
        status = val_target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_SOC_PERIPHERAL, SOC_PERIPHERAL_UART, instance),
                                       (uint8_t **)&uart_desc,
                                       (uint32_t *)sizeof(soc_peripheral_desc_t));
        if(status != TBSA_STATUS_SUCCESS) {
            return status;
        }

        if(uart_desc->stdio) {
            g_stdio_uart_base_addr = uart_desc->base;
            status = val_uart_init(g_stdio_uart_base_addr);
            if(status != TBSA_STATUS_SUCCESS) {
                return status;
            }
            break;
        }

        instance++;
    }while(instance < GET_NUM_INSTANCE(uart_desc));

    if(g_stdio_uart_base_addr == NULL) {
        return TBSA_STATUS_ERROR;
    }

    status = val_target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MEMORY, MEMORY_BOOTROM, 0),
                                   (uint8_t **)&bootrom_desc,
                                   (uint32_t *)sizeof(memory_desc_t));
    if(status != TBSA_STATUS_SUCCESS) {
        val_print(PRINT_ERROR, "\n\tBoot ROM address details not available in tbsa_tgt.cfg", 0);
        return status;
    }

    val_mem_reg_read(VTOR, &vtor);
    if ((vtor >= (uint32_t)(bootrom_desc->start)) && (vtor <= (uint32_t)(bootrom_desc->end))) {
        g_vtor_relocated_from_rom = FALSE;
    } else {
        g_vtor_relocated_from_rom = TRUE;
    }

    val_mem_reg_write(VTOR, (uint32_t)&g_tbsa_s_isr_vector);
    val_mem_reg_write(VTOR_NS, (uint32_t)&g_tbsa_ns_isr_vector);

    status = val_memory_init();
    if (status != TBSA_STATUS_SUCCESS) {
        return status;
    }

    status = val_nvram_init();
    if (status != TBSA_STATUS_SUCCESS) {
        return status;
    }

    status = val_get_test_binary_info(&g_test_binary_src_addr, &g_test_binary_in_ram);
    if (status != TBSA_STATUS_SUCCESS) {
        return status;
    }

    if (!g_test_binary_in_ram) {
        status = val_spi_init();
        if (status != TBSA_STATUS_SUCCESS) {
            return status;
        }
    }

    *test_id = val_nvram_get_last_id();

    return TBSA_STATUS_SUCCESS;
}

/**
    @brief    - Exit procedures for VAL infrastructure
    @param    - void
    @return   - tbsa_status_t
**/
tbsa_status_t val_infra_exit(void)
{
    tbsa_status_t status;
    memory_desc_t *memory_desc;
    test_count_t  test_count;

    status = val_target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MEMORY, MEMORY_NVRAM, 0),
                                   (uint8_t **)&memory_desc,
                                   (uint32_t *)sizeof(memory_desc_t));
    if(status != TBSA_STATUS_SUCCESS) {
        val_print(PRINT_ERROR, "\n\tNVRAM not present", 0);
        return status;
    }

    status = val_nvram_read(memory_desc->start, TBSA_NVRAM_OFFSET(NV_TEST_CNT), &test_count, sizeof(test_count_t));
    if(status != TBSA_STATUS_SUCCESS) {
        val_print(PRINT_ERROR, "\n\tNVRAM read error", 0);
        return status;
    }

    val_print(PRINT_ALWAYS, "\n\r", 0);
    for(int i=3; i < strlen(val_get_comp_name(CREATE_TEST_ID(TBSA_BASE_BASE, 1))); i++) {
        val_print(PRINT_ALWAYS, "-", 0);
    }

    val_print(PRINT_ALWAYS, "\n\rTotal tests : %d", (test_count.pass_cnt + test_count.skip_cnt + test_count.fail_cnt));
    val_print(PRINT_ALWAYS, "\n\rPass        : %d", test_count.pass_cnt);
    val_print(PRINT_ALWAYS, "\n\rFail        : %d", test_count.fail_cnt);
    val_print(PRINT_ALWAYS, "\n\rSkip        : %d", test_count.skip_cnt);

    val_print(PRINT_ALWAYS, "\n\r", 0);
    for(int i=3; i < strlen(val_get_comp_name(CREATE_TEST_ID(TBSA_BASE_BASE, 1))); i++) {
        val_print(PRINT_ALWAYS, "-", 0);
    }

   return TBSA_STATUS_SUCCESS;
}

/**
    @brief    - Parses input status for a given test and
                outputs appropriate information on the console
    @param    - test_id  : Current test ID
    @return   - tbsa_status_t
**/
tbsa_status_t val_report_status(test_id_t test_id)
{
    tbsa_status_buffer_t *psbuf = g_test_status_buffer;
    uint32_t status;

    if (psbuf == NULL) {
        val_print(PRINT_INFO, "No memory for status buffer\n", 0);
        return TBSA_STATUS_ERROR;
    }

    status = val_get_status();

    psbuf += test_id;

    psbuf->state = (status >> TEST_STATE_BIT) & TEST_STATE_MASK;
    psbuf->status = status & TEST_STATUS_MASK;

    switch(psbuf->state)
    {
        case TBSA_TEST_START:
            val_print(PRINT_INFO, "Test\t: 0x%x START\n\n", test_id);
            break;

        case TBSA_TEST_END:
            val_print(PRINT_INFO, "Test\t: 0x%x END\n\n", test_id);
            break;

        case TBSA_TEST_PASS:
            val_print(PRINT_TEST, "                                                               PASS", 0);
            break;

        case TBSA_TEST_FAIL:
            val_print(PRINT_TEST, "\n\r                                                                                 FAIL", 0);
            break;

        case TBSA_TEST_SKIP:
            val_print(PRINT_TEST, "\n\r                                                                                 SKIP", 0);
            break;

        case TBSA_TEST_PENDING:
            val_print(PRINT_TEST, "\n\t\t                                                                 PEND", 0);
            break;
    }

    return TBSA_STATUS_SUCCESS;
}

/**
    @brief    - Records the state and status of test
    @param    - status   : Test status
    @return   - tbsa_status_t
**/
tbsa_status_t val_set_status(uint32_t status)
{
    /* g_status_buf offset 0 is reserved for saving/checking test status temporarily */
    g_test_status_buffer[0].state   = ((status >> TEST_STATE_BIT) & TEST_STATE_MASK);
    g_test_status_buffer[0].status  = (status & TEST_STATUS_MASK);

    return TBSA_STATUS_SUCCESS;
}

/**
    @brief    - Returns the state and status of the test
    @param    - void
    @return   - test status
**/
uint32_t val_get_status(void)
{
    /* g_test_status_buffer offset 0 is reserved for saving/checking test status temporarily */
    return ((g_test_status_buffer[0].state) << TEST_STATE_BIT) | (g_test_status_buffer[0].status);
}

/*
    @brief    - This function checks if the input status argument is an error.
                On error, we print the checkpoint value and set the status.
    @param    - checkpoint      : Test debug checkpoint
              - tbsa_status_t   : Test status
    @return   - returns the input status back to the program.
*/
tbsa_status_t val_err_check_set(uint32_t checkpoint, tbsa_status_t status)
{
    if (TBSA_ERROR(status)) {
        val_print(PRINT_ERROR, "\n\r        Checkpoint %x : ", checkpoint);
        val_print(PRINT_ERROR, "Status = %x", status);
        val_set_status(RESULT_FAIL(status));
    } else {
        val_print(PRINT_DEBUG, "\n\r        Checkpoint %x : ", checkpoint);
    }

    return status;
}

/*
    @brief    - Generates system warm/cold reset.
    @param    - reset_type - Warm or Cold
    @return   - void
*/
void val_system_reset(system_reset_t reset_type)
{
    switch(reset_type)
    {
        case WARM_RESET:
            pal_system_warm_reset();
            break;

        case COLD_RESET:
            pal_system_cold_reset();
            break;

        default:
            return;
    }
}

/*
    @brief    - Reports the last reset type.
    @return   - TRUE/FALSE
*/
bool_t val_system_reset_type(system_reset_t reset_type)
{
    switch (reset_type)
    {
        case COLD_RESET:
            return pal_is_cold_reset();
        case WARM_RESET:
            return pal_is_warm_reset();
        case WDOG_RESET:
            return pal_is_wdog_reset();
        default:
            return UNKNOWN_RESET;
    }
}

/*
    @brief    - Reads the pre-defined component name against given test_id
    @param    - test_id  : Current Test ID
    @return   - Component name
*/
char *val_get_comp_name(test_id_t test_id)
{
    switch(TBSA_GET_COMP_NUM(test_id))
    {
        case TBSA_BASE_BASE:
            return TBSA_0_STR;
        case TBSA_TRUSTED_BOOT_BASE:
            return TBSA_1_STR;
        case TBSA_CRYPTO_BASE:
            return TBSA_2_STR;
        case TBSA_DEBUG_BASE:
            return TBSA_3_STR;
        case TBSA_EIP_BASE:
            return TBSA_4_STR;
        case TBSA_INTERRUPTS_BASE:
            return TBSA_5_STR;
        case TBSA_SECURE_RAM_BASE:
            return TBSA_6_STR;
        case TBSA_PERIPHERALS_BASE:
            return TBSA_7_STR;
        case TBSA_TRUSTED_TIMERS_BASE:
            return TBSA_8_STR;
        case TBSA_VERSION_COUNTERS_BASE:
            return TBSA_9_STR;
        default:
            return "No Component";

    }
}

/*
    @brief    - Reads the target configuration database to populate the
                tbsa_test_combined.bin source address and its physical
    @param    - test_binary_src_addr : physical location of tbsa_test_combined.bin file
                test_binary_in_ram   : 1 - if present in RAM, 0 - otherwise
                location(RAM or external storage).
    @return   - Component name
*/
tbsa_status_t val_get_test_binary_info(addr_t *test_binary_src_addr, uint32_t *test_binary_in_ram)
{
    tbsa_status_t        status;
    miscellaneous_desc_t *misc_desc;

    status = val_target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MISCELLANEOUS, MISCELLANEOUS_DUT, 0),
                                   (uint8_t **)&misc_desc,
                                   (uint32_t *)sizeof(miscellaneous_desc_t));

    if (status != TBSA_STATUS_SUCCESS) {
        *test_binary_src_addr = 0x0;
        *test_binary_in_ram   = 0x0;
        return status;
    }

    *test_binary_src_addr = misc_desc->test_binary_src_addr;
    *test_binary_in_ram   = misc_desc->test_binary_in_ram;

    return status;
}

/*
     @brief   - VAL facilitating memcpy support for tests
     @param   - dst  : destination address
                src  : source address
                size : number of bytes to copy
     @return  - void
*/
void val_memcpy(void *dst, void *src, uint32_t size)
{
    memcpy(dst, src, size);
}

/*
     @brief   - VAL facilitating memset support for tests
     @param   - dst  : destination address
                str  : value to set
                size : number of bytes to copy
     @return  - void
*/
void val_memset(void *dst, uint32_t str, uint32_t size)
{
    memset(dst, str, size);
}

/*
     @brief   - VAL facilitating execution of piece of trusted code at a given address
     @param   - addr : Trusted address
     @return  - Trusted code return value
*/
uint32_t val_execute_in_trusted_mode(addr_t address)
{
    typedef uint32_t (*fun_ptr)(void);
    fun_ptr fp;

    fp = (fun_ptr)address;

    return fp();
}

/*
     @brief   - Provides status of VTOR relocation before tbsa does
     @param   - void
     @return  - TRUE  - VTOR relocated from BOOT ROM
                FALSE - VTOR is not relocated from BOOT ROM
*/
bool_t val_is_vtor_relocated_from_rom(void)
{
    return g_vtor_relocated_from_rom;
}

/*
     @brief   - Check if the security extensions are enabled
     @param   - void
     @return  - TRUE  - Security extension are enabled
                FALSE - Security extension are disabled
*/
bool_t is_sec_ext_enabled(void)
{
    uint32_t tt_res, tt_adr;
    asm("mov %0, pc" : "=r"(tt_adr));
    asm("tt %0, %1": "=r"(tt_res) : "r" (tt_adr));
    if ((tt_res & BIT17) || (tt_res & BIT23))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
