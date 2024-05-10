/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __PMP_H__
#define __PMP_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define MAX_PMP_REGION              (16UL)

#if __riscv_xlen == 32
#define NB_PMP_CFG_REG              (4)
#define SIZE_PMP_CFG_REG            (4)
#elif __riscv_xlen == 64
#define NB_PMP_CFG_REG              (2)
#define SIZE_PMP_CFG_REG            (8)
#endif

#define PMP_CFG_1_BIT_MASK          (1)
#define PMP_CFG_2_BIT_MASK          (3)

#define PMP_READ_RIGHT_OFFSET       (0)
#define PMP_WRITE_RIGHT_OFFSET      (1)
#define PMP_EXECUTE_RIGHT_OFFSET    (2)
#define PMP_ADDRESS_RIGHT_OFFSET    (3)
#define PMP_LOCK_RIGHT_OFFSET       (7)

#define PMP_READ_RIGHT_MASK         (PMP_READ_RIGHT_OFFSET << \
                                    PMP_CFG_1_BIT_MASK)
#define PMP_WRITE_RIGHT_MASK        (PMP_WRITE_RIGHT_OFFSET << \
                                    PMP_CFG_1_BIT_MASK)
#define PMP_EXECUTE_RIGHT_MASK      (PMP_EXECUTE_RIGHT_OFFSET << \
                                    PMP_CFG_1_BIT_MASK)
#define PMP_ADDRESS_RIGHT_MASK      (PMP_CFG_2_BIT_MASK << \
                                    PMP_ADDRESS_RIGHT_OFFSET)
#define PMP_LOCK_RIGHT_MASK         (PMP_LOCK_RIGHT_OFFSET << \
                                    PMP_CFG_1_BIT_MASK)

/**
 * @brief pmp information
 */
typedef struct {
	/** @brief Number of PMP available on the hart */
	uint32_t nb_pmp;
	/** @brief smallest granularity available on the hart */
	uint32_t granularity;
} pmp_info_t ;

/**
 * @brief pmp configuration structure
 */
typedef struct {
	/** @brief read right configuration */
	uint8_t R;
	/** @brief write right configuration */
	uint8_t W;
	/** @brief execute right configuration */
	uint8_t X;
	/** @brief address matching configuration */
	uint8_t A;
	/** @brief lock mode */
	uint8_t L;
} pmp_cfg_t ;

/**
 * @brief PMP address strcuture
 */
typedef struct {
	/* address base value */
	size_t address;
	/* granularity value of the region */
	size_t granularity;
} pmp_addr_t ;

/**
 * @brief error code associated to PMP
 * @details the return value should be a signed 32 bits value, this way error
 * @details are coded as negative values, status code as positive values and
 * @details success is 0
 */
enum pmp_error_e {
	/* in case of success */
	PMP_SUCCESS = 0,
	/* default error, the cause is blurry (probably undesired execution path) */
	PMP_DEFAULT_ERROR = (int)0x80000000,
	/* invalid pointer */
	PMP_INVALID_POINTER,
	/* error system garnularity incompatible  */
	PMP_ERR_GRANULARITY,
	/* invalid NAPOT size (not power of 2) */
	PMP_INVALID_NAPOT_SIZE,
	/* unaligned address */
	PMP_UNALIGNED_ADDRESS,
	/* invalid parameter */
	PMP_INVALID_PARAM,
	/* unsupported */
	PMP_ERR_UNSUPPORTED,
};

/**
 * @brief address mode
 */
enum pmp_address_mode_e {
	/* Null Region */
	PMP_OFF = 0,
	/* Top of Range mode */
	PMP_TOR,
	/* Naturally aligned four-bytes region */
	PMP_NA4,
	/* Naturally aligned power-of-two region, >= 8 bytes */
	PMP_NAPOT,
};

/**
 * @brief get the number of pmp for the hart and granularity
 *
 * @param pmp_info  structure that contains numbre of pmp and granularity
 * @return int32_t  0 on success
 * @return int32_t  < 0 on failure
 */
int32_t init_pmp(pmp_info_t *pmp_info);

/**
 * @brief Set the pmp config object
 *
 * @param[in] config            PMP configuration structure
 * @param[out] register_val     config value of the region in pmpcfgx
 * @return int32_t  0 in case of success
 * @return int32_t  <0 otherwise
 */
int32_t set_pmp_config(pmp_cfg_t *config, uint8_t *register_val);

/**
 * @brief Get the pmp config object
 *
 * @param[in] register_val      config value of the region in pmpcfgx
 * @param[out] config           PMP configuration structure
 * @return int32_t  0 in case of success
 * @return int32_t  <0 otherwise
 */
int32_t get_pmp_config(uint8_t *register_val, pmp_cfg_t *config);

/**
 * @brief
 *
 * @param[in] pmp_info      PMP information (number of PMP and hart granularity)
 * @param[in] region        PMP region to configure
 * @param[in] pmp_config    PMP configuration value (byte in pmpcfx)
 * @param[in] address       Value that will be set in pmpaddrx of the PMP region
 * @return int32_t  0 in case of success
 * @return int32_t  <0 otherwise
 */
int32_t write_pmp_config(pmp_info_t *pmp_info, uint32_t region,
						 uint8_t pmp_config, size_t address);

/**
 * @brief
 *
 * @param[in] pmp_info      PMP information (number of PMP and hart granularity)
 * @param[in] region        PMP region from which to read configuration
 * @param[out] pmp_config   PMP configuration value (byte in pmpcfx)
 * @param[out] address      value that will be read from pmpaddrx of the PMP region
 * @return int32_t  0 in case of success
 * @return int32_t  <0 otherwise
 */
int32_t read_pmp_config(pmp_info_t *pmp_info, uint32_t region,
						uint8_t *pmp_config, size_t *address);

/**
 * @brief write all PMP pmpcfgx registers in one time
 *
 * @param[in] configs       PMP config table (pmpcfx table)
 * @param[in] pmp_region    number of pmp regions available
 * @return int32_t  0 in case of success
 * @return int32_t  <0 otherwise
 * @warning configs address should be aligned (32 bits for RV32 64 bits for RV64)
 * @warning RV64)
 * @warning address table should be of
 * @warning sizeof(size_t)*(pmp_region / SIZE_PMP_CFG_REG) length
 */
int32_t write_pmp_configs(size_t *configs, uint32_t pmp_region);
/**
 * @brief read all PMP pmpcfgx registers in one time
 *
 * @param[out] configs      PMP config table (pmpcfx table)
 * @param[in] pmp_region    number of pmp regions available
 * @return int32_t  0 in case of success
 * @return int32_t  <0 otherwise
 * @warning configs address should be aligned (32 bits for RV32 64 bits for RV64)
 * @warning RV64)
 * @warning address table should be of
 * @warning sizeof(size_t)*(pmp_region / SIZE_PMP_CFG_REG) length
 */
int32_t read_pmp_configs(size_t *configs, uint32_t pmp_region);

/**
 * @brief write all PMP pmpaddrx in one time
 *
 * @param[in] address       PMP address table (pmpaddrx table)
 * @param[in] pmp_region    number of pmp regions available
 * @return int32_t  0 in case of success
 * @return int32_t  <0 otherwise
 * @warning address table should be of sizeof(size_t)*pmp_region length
 */
int32_t write_pmp_addrs(size_t *address, uint32_t pmp_region);

/**
 * @brief read all PMP pmpaddrx in one time
 *
 * @param[out] address      PMP address table (pmpaddrx table)
 * @param[in] pmp_region    number of pmp regions available
 * @return int32_t  0 in case of success
 * @return int32_t  <0 otherwise
 * @warning address table should be of sizeof(size_t)*pmp_region length
 */
int32_t read_pmp_addrs(size_t *address, uint32_t pmp_region);

/**
 * @brief compute the pmpaddrx for NA4 and TOR mode (do granularity checks)
 *
 * @param[in]   granularity     Memory granularity in byte
 * @param[in]   address_in      Base address of the region
 * @param[out]  address_out     address compatible with NA4 or TOR style (can be
 *                              use into pmpaddrx regs)
 * @return int32_t  0 in case of success
 * @return int32_t  <0 otherwise
 */
int32_t addr_modifier(size_t granularity,
					  size_t address_in,
					  size_t *address_out);

/**
 * @brief compute the pmpaddrx for NAPOT mode (do granularity checks)
 *
 * @param[in]   granularity     Memory granularity in byte
 * @param[in]   address_in      Base address of the region
 * @param[out]  address_out     address NAPOT style (can be use into pmpaddrx
 *                              regs)
 * @param[in]   size            Size of the region
 * @return int32_t  0 in case of success
 * @return int32_t  <0 otherwise
 */
int32_t napot_addr_modifier(size_t granularity,
							size_t address_in,
							size_t *address_out,
							size_t size);

#endif  // __PMP_H__