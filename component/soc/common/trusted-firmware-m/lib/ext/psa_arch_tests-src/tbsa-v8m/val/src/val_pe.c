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

#include "val_infra.h"
#include "val_test.h"
#include "val_pe.h"

/**
    @brief    - This function will return the value read from a memory mapped register.
    @param    - reg_id  : memory mapped address of the system register
                rd_data : data read
    @return   - TBSA_STATUS_SUCCUSS on successful execution of the function, and fail error
                codes if incorrect args are passed
**/
tbsa_status_t val_mem_reg_read(uint32_t reg_id, uint32_t *rd_data)
{
  return val_mem_read((uint32_t *)reg_id, WORD, rd_data);
}

/**
    @brief    - This function will return the value read from a PE system register.
    @param    - reg_id  : enum of the CPU system register as defined in sys_reg_t
                rd_data : data read
    @return   - TBSA_STATUS_SUCCUSS on successful execution of the function, and fail error
                codes if incorrect args are passed
**/
tbsa_status_t val_pe_reg_read(sys_reg_t reg_id, uint32_t *rd_data)
{
  switch(reg_id) {
      case CONTROL:
        *rd_data = sysread_control_asm();
        break;
      case CONTROL_NS:
        *rd_data = sysread_control_ns_asm();
        break;
      default:
        return TBSA_STATUS_INVALID_ARGS;
  }
  return TBSA_STATUS_SUCCESS;
}

/**
    @brief    - This function will return the value read from a memory based on the access size
    @param    - address     : address to be read
                access_size : access size - BYTE/HALF_WORD/WORD
                rd_data     : data read
    @return   - TBSA_STATUS_SUCCUSS on successful execution of the function, and fail error
                codes if incorrect args are passed
**/
tbsa_status_t val_mem_read(uint32_t *address, mem_access_size_t access_size, uint32_t *rd_data)
{
  switch(access_size) {
    case BYTE:
      *rd_data = ldrb_asm(address);
      return TBSA_STATUS_SUCCESS;
    case HALF_WORD:
      *rd_data = ldrh_asm(address);
      return TBSA_STATUS_SUCCESS;
    case WORD:
      *rd_data = ldr_asm(address);
      return TBSA_STATUS_SUCCESS;
    default:
      return TBSA_STATUS_INVALID_ARGS;

  }
}

/**
    @brief    - This function will return the value read from a memory(uses ldr.w)
    @param    - address     : address to be read
                rd_data     : data read
    @return   - TBSA_STATUS_SUCCUSS
**/
tbsa_status_t val_mem_read_wide(uint32_t *address, uint32_t *rd_data)
{
    *rd_data = ldr_wide_asm(address);
    return TBSA_STATUS_SUCCESS;
}

/**
    @brief    - This function will write the value to the specified address(uses str.w)
    @param    - address     : address to be written
                wr_data     : data to be written with
    @return   - TBSA_STATUS_SUCCUSS
**/
tbsa_status_t val_mem_write_wide(uint32_t *address, uint32_t wr_data)
{
    str_wide_asm(address, wr_data);
    return TBSA_STATUS_SUCCESS;
}

/**
    @brief    - This function will write a value to a memory mapped register.
    @param    - reg_id  : memory mapped address of the system register
                wr_data : data to be written with
    @return   - TBSA_STATUS_SUCCUSS on successful execution of the function, and fail error
                codes if incorrect args are passed
**/
tbsa_status_t val_mem_reg_write(uint32_t reg_id, uint32_t wr_data)
{
    tbsa_status_t status;
    status = val_mem_write((uint32_t *)reg_id, WORD, wr_data);
    dsb_asm();
    isb_asm();
    return status;
}

/**
    @brief    - This function will write a value to a system register.
    @param    - reg_id  : enum of the CPU system register as defined in sys_reg_t
                wr_data : data to be written with
    @return   - TBSA_STATUS_SUCCUSS on successful execution of the function, and fail error
                codes if incorrect args are passed
**/
tbsa_status_t val_pe_reg_write(sys_reg_t reg_id, uint32_t wr_data)
{
  switch(reg_id) {
      case CONTROL:
        syswrite_control_asm(wr_data);
        break;
      case CONTROL_NS:
        syswrite_control_ns_asm(wr_data);
        break;
      default:
        return TBSA_STATUS_INVALID_ARGS;
  }
  return TBSA_STATUS_SUCCESS;
}

/**
    @brief    - This function will write a value to a memory based on access size.
    @param    - address     : address to be written
                access_size : access size - BYTE/HALF_WORD/WORD
                wr_data     : data to be written with
    @return   - TBSA_STATUS_SUCCUSS on successful execution of the function, and fail error
                codes if incorrect args are passed
**/
tbsa_status_t val_mem_write(uint32_t *address, mem_access_size_t access_size, uint32_t wr_data)
{
    switch(access_size) {
        case BYTE:
            strb_asm(address, wr_data);
            return TBSA_STATUS_SUCCESS;
        case HALF_WORD:
            strh_asm(address, wr_data);
            return TBSA_STATUS_SUCCESS;
        case WORD:
            str_asm(address, wr_data);
            return TBSA_STATUS_SUCCESS;
        default:
            return TBSA_STATUS_INVALID_ARGS;
    }
}

/**
    @brief    - This function will return the TT response result for a given address
    @param    - address     : address
    @return   - 1 Target address is Secure
                0 Target address is Non-secure
**/
bool_t val_is_secure_address(addr_t address)
{
    uint32_t result;
    result = tt_asm((uint32_t*)address);

    return ((result & (1UL<<22))?1:0);
}
