/** @file
 * Copyright (c) 2018, Arm Limited or its affiliates. All rights reserved.
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

#ifndef _VAL_TEST_H_
#define _VAL_TEST_H_

#include "val_common.h"

#define ELF_IDENT                       16
#define TBSA_TEST_INVALID               0xFFFFFFFF
#define TBSA_TEST_START_MARKER          0xfaceface
#define TBSA_TEST_END_MARKER            0xc3c3c3c3


#define TEST_CHECKPOINT_1               0xC01
#define TEST_CHECKPOINT_2               0xC02
#define TEST_CHECKPOINT_3               0xC03
#define TEST_CHECKPOINT_4               0xC04
#define TEST_CHECKPOINT_5               0xC05
#define TEST_CHECKPOINT_6               0xC06
#define TEST_CHECKPOINT_7               0XC07
#define TEST_CHECKPOINT_8               0xC08
#define TEST_CHECKPOINT_9               0xC09
#define TEST_CHECKPOINT_A               0xC0A
#define TEST_CHECKPOINT_B               0xC0B
#define TEST_CHECKPOINT_C               0xC0C
#define TEST_CHECKPOINT_D               0xC0D
#define TEST_CHECKPOINT_E               0xC0E
#define TEST_CHECKPOINT_F               0xC0F
#define TEST_CHECKPOINT_10              0xC10
#define TEST_CHECKPOINT_11              0xC11
#define TEST_CHECKPOINT_12              0xC12
#define TEST_CHECKPOINT_13              0xC13
#define TEST_CHECKPOINT_14              0xC14
#define TEST_CHECKPOINT_15              0xC15
#define TEST_CHECKPOINT_16              0xC16
#define TEST_CHECKPOINT_17              0xC17
#define TEST_CHECKPOINT_18              0xC18
#define TEST_CHECKPOINT_19              0xC19
#define TEST_CHECKPOINT_20              0xC20
#define TEST_CHECKPOINT_21              0xC21
#define TEST_CHECKPOINT_22              0xC22

#define TBSA_TOTAL_TESTS                200
#define TBSA_TEST_STATUS_BUF_SIZE       (TBSA_TOTAL_TESTS * sizeof(tbsa_status_buffer_t))

#define TBSA_TEST_START                 0x01
#define TBSA_TEST_END                   0x02
#define TBSA_TEST_PASS                  0x04
#define TBSA_TEST_FAIL                  0x08
#define TBSA_TEST_SKIP                  0x10
#define TBSA_TEST_PENDING               0x20

#define TEST_NUM_BIT                    32
#define TEST_STATE_BIT                  8
#define TEST_STATUS_BIT                 0

#define TEST_NUM_MASK                   0xFFFFFFFF
#define TEST_STATE_MASK                 0xFF
#define TEST_STATUS_MASK                0xFF

#define RESULT_PASS(status)             (((TBSA_TEST_PASS) << TEST_STATE_BIT) | ((status) << TEST_STATUS_BIT))
#define RESULT_FAIL(status)             (((TBSA_TEST_FAIL) << TEST_STATE_BIT) | ((status) << TEST_STATUS_BIT))
#define RESULT_SKIP(status)             (((TBSA_TEST_SKIP) << TEST_STATE_BIT) | ((status) << TEST_STATUS_BIT))
#define RESULT_PENDING(status)          (((TBSA_TEST_PENDING) << TEST_STATE_BIT) | ((status) << TEST_STATUS_BIT))

#define IS_TEST_FAIL(status)            (((status >> TEST_STATE_BIT) & TEST_STATE_MASK) == TBSA_TEST_FAIL)
#define IS_TEST_PASS(status)            (((status >> TEST_STATE_BIT) & TEST_STATE_MASK) == TBSA_TEST_PASS)
#define IS_TEST_SKIP(status)            (((status >> TEST_STATE_BIT) & TEST_STATE_MASK) == TBSA_TEST_SKIP)
#define IS_TEST_PENDING(status)         (((status >> TEST_STATE_BIT) & TEST_STATE_MASK) == TBSA_TEST_PENDING)

#define TBSA_STATUS(status)             (status & TEST_STATUS_MASK)

#define TBSA_ELF_IDENT                  16

/* typedef's */
typedef enum {
    ENTRY_FUNC_HOOK_S  = 0,
    PAYLOAD_FUNC_S     = 1,
    EXIT_FUNC_HOOK_S   = 2,
    ENTRY_FUNC_HOOK_NS = 3,
    PAYLOAD_FUNC_NS    = 4,
    EXIT_FUNC_HOOK_NS  = 5
}tbsa_test_fn_type_t;

typedef struct
{
    uint32_t  start_marker;
    test_id_t test_id;
    uint32_t  s_elf_size;
    uint32_t  ns_elf_size;
}tbsa_test_header_t;

typedef struct
{
    uint16_t reserved;
    uint8_t  state;
    uint8_t  status;
}tbsa_status_buffer_t;

typedef struct {
    uint32_t *bss_start;
    uint32_t *bss_end;
}tbsa_test_init_t;

typedef uint32_t  elf32_word;
typedef int32_t   elf32_sword;
typedef uint16_t  elf32_half;
typedef uint32_t  elf32_off;
typedef uint32_t  elf32_addr;

typedef struct {
    unsigned char e_ident[ELF_IDENT]; /* ident bytes */
    elf32_half    e_type;            /* file type */
    elf32_half    e_machine;         /* target machine */
    elf32_word    e_version;         /* file version */
    elf32_addr    e_entry;           /* start address */
    elf32_off     e_phoff;           /* phdr file offset */
    elf32_off     e_shoff;           /* shdr file offset */
    elf32_word    e_flags;           /* file flags */
    elf32_half    e_ehsize;          /* sizeof ehdr */
    elf32_half    e_phentsize;       /* sizeof phdr */
    elf32_half    e_phnum;           /* number phdrs */
    elf32_half    e_shentsize;       /* sizeof shdr */
    elf32_half    e_shnum;           /* number shdrs */
    elf32_half    e_shstrndx;        /* shdr string index */
}tbsa_elf_header_t;

typedef struct {
    elf32_word p_type;          /* Segment type */
    elf32_off  p_offset;        /* Segment file offset */
    elf32_addr p_vaddr;         /* Segment virtual address */
    elf32_addr p_paddr;         /* Segment physical address */
    elf32_word p_filesz;        /* Segment size in file */
    elf32_word p_memsz;         /* Segment size in memory */
    elf32_word p_flags;         /* Segment flags */
    elf32_word p_align;         /* Segment alignment */
}tbsa_pheader_t;

/* prototypes */
tbsa_status_t val_test_get_info   (test_id_t test_id, tbsa_test_fn_type_t info_type, addr_t *paddr);
tbsa_status_t val_test_load       (test_id_t *test_id, test_id_t test_id_prev);
tbsa_status_t val_test_initialize (tbsa_test_init_t *init);

#endif /* _VAL_TEST_H_ */

