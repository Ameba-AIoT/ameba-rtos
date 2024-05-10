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

#ifndef _VAL_TARGET_H_
#define _VAL_TARGET_H_

#ifndef _STDIO_H_
#include "val_common.h"
#endif

#define TARGET_CONFIG_CREATE_ID(major, minor, index) \
                    (((major & 0xFF) << 24) | ((minor & 0xFF) << 16) | (index & 0xFFFF))
#define TARGET_CONFIG_GET_MAJOR(config_id) ((config_id >> 24) & 0xFF)
#define TARGET_CONFIG_GET_MINOR(config_id) ((config_id >> 16) & 0xFF)

#define GET_NUM_INSTANCE(struct_type)   (struct_type->cfg_type.size >> 24)

#define DPM_PW_SIZE_BYTES 32

#define TARGET_MIN_CFG_ID TARGET_CONFIG_CREATE_ID(GROUP_SOC_PERIPHERAL, 0, 0)
#define TARGET_MAX_CFG_ID TARGET_CONFIG_CREATE_ID(GROUP_MAX, 0, 0)

/**
    Config IDs for each group/component
    31:24 : MAJOR (group)
    23:16 : MINOR (component)
    16:8  : SUB-component
    7:0  : INSTANCE (instance of same component)
**/
/*
   MAJOR IDs
*/
typedef enum _GROUP_CONFIG_ID_ {
  GROUP_SOC_PERIPHERAL     = 0x1,// Peripheral group
  GROUP_MEMORY             = 0x2,//Memory group
  GROUP_CRYPTO             = 0x3,
  GROUP_NETWORK_PERIPHERAL = 0x4,// Peripheral group
  GROUP_CLOCKS             = 0x5,
  GROUP_PROTECTION_UNITS   = 0x6,
  GROUP_KEY                = 0x7,
  GROUP_MISCELLANEOUS      = 0x8,
  GROUP_DPM                = 0x9,
  GROUP_FUSE               = 0xA,
  GROUP_MAX                = 0xFF,
} group_cfg_id_t;

/*
   MINOR IDs
 */
typedef enum _SOC_PERIPHERAL_CONFIG_ID_ {
  SOC_PERIPHERAL_UART      = 0x1,
  SOC_PERIPHERAL_TIMER     = 0x2,
  SOC_PERIPHERAL_WATCHDOG  = 0x3,
  SOC_PERIPHERAL_RTC       = 0x4,
  SOC_PERIPHERAL_INTRCNT   = 0x5,
  SOC_PERIPHERAL_SPI       = 0x6
} soc_peripheral_cfg_id_t;

typedef enum _MEMORY_CONFIG_ID_ {
  MEMORY_SRAM              = 0x1,
  MEMORY_FLASH             = 0x2,
  MEMORY_NVRAM             = 0x3,
  MEMORY_BOOTROM           = 0x4
} memory_cfg_id_t;

typedef enum _CRYPTO_CONFIG_ID_ {
  CRYPTO_CRYPTO            = 0x1
} crypto_cfg_id_t;

typedef enum _CLOCKS_CONFIG_ID_ {
  CLOCKS_SYS_FREQ          = 0x1
} clocks_cfg_id_t;

typedef enum _PROTECTTION_UNITS_CONFIG_ID_ {
  PROTECTION_UNITS_IDAU    = 0x1,
  PROTECTION_UNITS_MSC     = 0x2,
  PROTECTION_UNITS_PPC     = 0x3,
  PROTECTION_UNITS_MPC     = 0x4
} protection_units_cfg_id_t;

typedef enum _KEY_CONFIG_ID_ {
  KEY_KEY                  = 0x1,
} key_cfg_id_t;

typedef enum _FUSE_CONFIG_ID_ {
  FUSE_FUSE                = 0x1,
} fuse_cfg_id_t;

typedef enum _MISCELLANEOUS_CONFIG_ID_ {
  MISCELLANEOUS_VER_COUNT  = 0x1,
  MISCELLANEOUS_DUT        = 0x2
} miscellaneous_cfg_id_t;

typedef enum _DPM_CONFIG_ID_ {
  DPM_DPM                  = 0x1,
} dpm_cfg_id_t;

/**
  Assign group type to each system component
**/
typedef enum _COMPONENT_GROUPING_{
  UART              = GROUP_SOC_PERIPHERAL,
  TIMER             = GROUP_SOC_PERIPHERAL,
  WATCHDOG          = GROUP_SOC_PERIPHERAL,
  RTC               = GROUP_SOC_PERIPHERAL,
  INTRCNT           = GROUP_SOC_PERIPHERAL,
  SRAM              = GROUP_MEMORY,
  FLASH             = GROUP_MEMORY,
  NVRAM             = GROUP_MEMORY,
  BOOTROM           = GROUP_MEMORY,
  CRYPTO            = GROUP_CRYPTO,
  SYS_FREQ          = GROUP_CLOCKS,
  IDAU              = GROUP_PROTECTION_UNITS,
  MSC               = GROUP_PROTECTION_UNITS,
  PPC               = GROUP_PROTECTION_UNITS,
  MPC               = GROUP_PROTECTION_UNITS,
  KEY               = GROUP_KEY,
  FUSE              = GROUP_FUSE,
  VER_COUNT         = GROUP_MISCELLANEOUS,
  DUT               = GROUP_MISCELLANEOUS,
  DPM               = GROUP_DPM,
} comp_group_assign_t;



/**
  Target Configuration Header
**/
typedef struct _TARGET_CFG_HDR_ {
    uint32_t  signature[2];       //TBSA_CFG
    uint32_t  target_string[2];  //8 byte String describing the Target platform
    uint32_t  version;    //1 for now
    uint32_t  size;   //including the header size
}target_cfg_hdr_t;

typedef enum {
    FUSE_CONFIDENTIAL      = 0x001,
    FUSE_LOCKABLE          = 0x002,
    FUSE_LOCKED            = 0x004,
    FUSE_PUBLIC            = 0x008,
    FUSE_USER              = 0x010,
    FUSE_HW_IP             = 0x020,
    FUSE_OPEN              = 0x040,
    FUSE_BITWISE           = 0x080,
    FUSE_BULK              = 0x100
} fuse_type_t;

/**
  !* Life Cycle State *!
  Chip manufacturer (CM LCS)
  Device manufacturer (DM LCS)
  Security enabled (Secure LCS)
  RMA (RMA LCS)
**/
typedef enum {
    LCS_CM                 = 0x0,
    LCS_DM                 = 0x1,
    LCS_SEC_ENABLED        = 0x5,
    LCS_RMA                = 0x7,
} lcs_state_t;

/* Defines for 'type' member in _KEY_INFO_DESC_ struct */
typedef enum {
    ROTPK                  = 0x0100000,
    HUK                    = 0x0200000,
    TRUST                  = 0x0400000,
    STATIC                 = 0x0800000,
    REVOKE                 = 0x1000000,
} key_type_t;


typedef enum {
    TRUSTED                = 0x1,
    NON_TRUSTED            = 0x2,
}firmware_version_type_t;

typedef enum {
    SECURE_ACCESS          = 0x1,
    NONSECURE_ACCESS       = 0x2,
    SECURE_PROGRAMMABLE    = 0x3,
    NONSECURE_PROGRAMMABLE = 0x4,
}dev_attr_t;

typedef enum {
    MEM_SECURE             = 0x0,
    MEM_NONSECURE          = 0x1,
    MEM_NSC                = 0x2,
    MEM_CONFIGURABLE       = 0x3,
    MEM_UNUSED             = 0x4
}mem_tgt_attr_t;

typedef enum {
    TYPE_DEVICE            = 0x1,  /* NOTE: This define is just kept to differentiate between device and normal memory,
                            it should not be used for device/peripherals. Use soc_peripherals_desc for device details*/
    TYPE_NORMAL_READ_ONLY  = 0x2,
    TYPE_NORMAL_WRITE_ONLY = 0x3,
    TYPE_NORMAL_READ_WRITE = 0x4,
    TYPE_NORMAL_EXECUTE    = 0x5,
}mem_type_t;

typedef enum {
    TOKEN_PASSWD           = 0x1,
    TOKEN_CERTIFICATE      = 0x2,
    TOKEN_CHALLENGE_RESP   = 0x3,
} unlock_token_t;

typedef enum {
    SECURE_TARGET_ONLY      = 0x1,
    NONSECURE_TARGET_ONLY   = 0x2,
    SECURE_NONSECURE_TARGET = 0x4,
} interrupt_target_t;

typedef struct _CFG_HDR_TYPE_ {
    uint32_t cfg_id;
    uint32_t size;  //size inclusive of this header
} cfg_type_t;

/**
  PE Information
**/
typedef struct _PE_INFO_NUM_ {
    cfg_type_t   cfg_type;
    uint32_t  num_pe;
}cfg_pe_num_t;


/**
   Memory Information
**/
typedef struct _MEM_INFO_DESC_ {
    cfg_type_t  cfg_type;
    uint32_t    num;
} memory_hdr_t;

typedef struct _MEM_REGION_ {
    cfg_type_t     cfg_type;
    addr_t         start;
    addr_t         end;
    addr_t         mpc;
    uint32_t       s_offset;
    uint32_t       ns_offset;
    mem_tgt_attr_t attribute;
    /* Add the DPM index controlling the address range */
    /* The ordering changed to avoid compiler packing */
    uint32_t   dpm_index;
    mem_type_t mem_type;
} memory_desc_t;


/**
  Protection units
**/
typedef struct _PROT_UNIT_HDR_DESC_ {
    cfg_type_t cfg_type;
    uint32_t   num;
} protection_units_hdr_t ;

typedef struct _PROT_UNIT_INFO_DESC_ {
    cfg_type_t     cfg_type;
    addr_t         device_base;
    dev_attr_t     attribute;
    uint32_t       intr_id;
    addr_t         start;
    addr_t         end;
    mem_tgt_attr_t mem_attr;
} protection_units_desc_t;

/**
  CRYPTO Information
**/
typedef enum {
    ECC            = 0x1000,
    RSA            = 0x2000,
    DIFFIE_HELLMAN = 0x4000,
    NONE           = 0x8000,
    ASM_MSK        = 0xF000,
    HASH           = 0x10000,
    AES            = 0x20000,
    DES            = 0x40000,
    SYM_MSK        = 0xF0000,
} crypt_t;

typedef struct _CRYPTO_INFO_NUM_ {
    cfg_type_t cfg_type;
    uint32_t   num;
} crypto_hdr_t;

typedef struct _CRYPTO_INFO_DESC_ {
    cfg_type_t    cfg_type;
    uint32_t      index;
    addr_t        base;
    uint32_t      lcs;
    dev_attr_t    attribute;
    uint32_t      vendor_id;
    uint32_t      device_id;
    uint32_t      supported_alg;
    uint32_t      intr_id;
} crypto_desc_t;

/*
   SOC Peripheral description structures
*/
typedef struct _SOC_PER_INFO_NUM_ {
    cfg_type_t cfg_type;
    uint32_t   num;
} soc_peripheral_hdr_t;

typedef struct _SOC_PER_INFO_DESC_ {
    cfg_type_t         cfg_type;
    uint32_t           vendor_id;
    uint32_t           device_id;
    addr_t             base;
    uint32_t           offset;
    uint32_t           intr_id;
    interrupt_target_t intr_target;
    addr_t             clk_src;
    uint32_t           stdio;
    dev_attr_t         attribute;
} soc_peripheral_desc_t;

/*
   Network Peripheral description structures
*/
typedef struct _NETWORK_PER_INFO_NUM_ {
    cfg_type_t cfg_type;
    uint32_t   num;
} network_peripheral_hdr_t;

typedef struct _NETWORK_PER_INFO_DESC_ {
    cfg_type_t  cfg_type;
    uint32_t    vendor_id;
    uint32_t    device_id;
    addr_t      base;
    uint32_t    intr_id;
    dev_attr_t  attribute;
} network_peripheral_desc_t;


/**
  System Clock Information
**/

typedef struct _CLOCKS_HDR_INFO_ {
    cfg_type_t cfg_type;
    uint32_t   num;
} clocks_hdr_t;

typedef struct _SYS_FREQ_INFO_ {
    cfg_type_t cfg_type;
    uint32_t   sys_freq;   /* in MHz */
    addr_t     pll_base;
    uint32_t   offset;
} clocks_desc_t;

/**
  Crypto Key Information
**/

typedef struct _KEY_INFO_HDR_ {
    cfg_type_t cfg_type;
    uint32_t   num;
} key_hdr_t;

typedef struct _KEY_INFO_DESC_ {
    cfg_type_t   cfg_type;
    uint32_t     type;
    uint32_t     size;  /* in words */
    addr_t       addr;
    uint32_t     index;
    uint32_t     state;
    uint32_t     def_val;
} key_desc_t;

/**
  Fuse Information
**/

typedef struct _FUSE_INFO_HDR_ {
    cfg_type_t cfg_type;
    uint32_t   num;
} fuse_hdr_t;

typedef struct _FUSE_INFO_DESC_ {
    cfg_type_t   cfg_type;
    uint32_t     type;
    uint32_t     size;
    addr_t       addr;
    uint32_t     def_val;
} fuse_desc_t;

/**
  System Miscellaneous Information
**/

typedef struct _MISCELLANEOUS_INFO_HDR_ {
    cfg_type_t cfg_type;
    uint32_t   num;
} miscellaneous_hdr_t;

typedef struct _MISCELLANEOUS_INFO_DESC_ {
    cfg_type_t              cfg_type;
    addr_t                  test_binary_src_addr;
    uint32_t                test_binary_in_ram;
    addr_t                  s_addr;
    addr_t                  ns_addr;
    addr_t                  nsc_addr;
    addr_t                  s_test_addr;
    addr_t                  ns_test_addr;
    firmware_version_type_t fw_ver_type;
    uint32_t                fw_ver_cnt_max;
} miscellaneous_desc_t;

/**
  DPM Information
**/

typedef struct _DPM_INFO_HDR_ {
    cfg_type_t cfg_type;
    uint32_t   num;
} dpm_hdr_t;

typedef struct _DPM_INFO_DESC_ {
    cfg_type_t      cfg_type;
    uint32_t        index;  // Unique index to identify each DPM
    unlock_token_t  unlock_token;
    uint32_t        is_controlling_secure;
    addr_t          data_addr;
    addr_t          flag_addr;
    union {
        uint8_t     passwd[DPM_PW_SIZE_BYTES];
        addr_t      certificate_addr;
    };
    addr_t          public_key_addr;
    union {     /*size in bytes*/
        uint32_t    passwd_size;
        uint32_t    certificate_size;
    };
    uint32_t        public_key_size;
    crypt_t         certificate_unlock_algo;
} dpm_desc_t;

#endif /* _VAL_TARGET_H_ */
