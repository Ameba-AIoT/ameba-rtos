/** @file
 * Copyright (c) 2021 Arm Limited or its affiliates. All rights reserved.
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

#ifndef _VAL_DISPATCHER_H_
#define _VAL_DISPATCHER_H_

#include "val.h"

#define ELF_IDENT                      16
#define VAL_INVALID_TEST_ID            0xffffffff
#define VAL_TEST_START_MARKER          0xfaceface
#define VAL_TEST_END_MARKER            0xc3c3c3c3

/* typedef's */
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
} elf_header_t;

typedef struct {
    elf32_word p_type;          /* Segment type */
    elf32_off  p_offset;        /* Segment file offset */
    elf32_addr p_vaddr;         /* Segment virtual address */
    elf32_addr p_paddr;         /* Segment physical address */
    elf32_word p_filesz;        /* Segment size in file */
    elf32_word p_memsz;         /* Segment size in memory */
    elf32_word p_flags;         /* Segment flags */
    elf32_word p_align;         /* Segment alignment */
} elf_pheader_t;

typedef struct {
    uint32_t  start_marker;
    test_id_t test_id;
    uint32_t  elf_size;
} test_header_t;

int32_t val_dispatcher(test_id_t test_id_prev);
#endif
