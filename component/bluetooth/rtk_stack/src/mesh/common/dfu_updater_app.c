/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      dfu_updater_app.c
* @brief     Smart mesh dfu application
* @details
* @author    bill
* @date      2018-6-5
* @version   v1.0
* *********************************************************************************************************
*/

#define MM_ID MM_APP

#if F_BT_MESH_1_1_DFU_SUPPORT

#include "dfu_updater_app.h"
#include "mem_config.h"
#include "dfu_api.h"
#include "flash_device.h"
#include "patch_header_check.h"
#include "silent_dfu_flash.h"
#include "platform_ic_misc.h"
#include "generic_types.h"

#if DFU_UPDATER_SUPPORT_POWER_OFF_GO_ON /* used for device power off but client is alive */
typedef struct
{
    uint8_t blob_id[8];
    uint32_t blob_size;
    uint32_t block_size;
    uint32_t current_block_num; /**< shall be 4 bytes aligned */
    uint16_t transfer_timeout_base;
    uint16_t transfer_ttl;
    uint16_t transfer_mtu_size;
    uint16_t updater_addr;
    uint16_t updater_app_key_index;
    uint16_t image_id;
    uint8_t transfer_mode;
    uint8_t padding[3];
} dfu_updater_nvm_info_t;
static dfu_updater_nvm_info_t dfu_updater_nvm_info;

void dfu_updater_nvm_load(void)
{
    uint32_t ret = ftl_load(&dfu_updater_nvm_info, DFU_UPDATER_NVM_OFFSET,
                            sizeof(dfu_updater_nvm_info));
    if (ret == 0 && dfu_updater_nvm_info.blob_size != 0)
    {
        printi("dfu_updater: power on go on!");
    }
    else
    {
        dfu_updater_nvm_info.blob_size = 0;
    }
}

void dfu_updater_nvm_clear(void)
{
    uint32_t ret;
    ret = ftl_load(&dfu_updater_nvm_info.blob_size,
                   DFU_UPDATER_NVM_OFFSET + MEMBER_OFFSET(dfu_updater_nvm_info_t, blob_size), 4);
    if (ret == 0 && dfu_updater_nvm_info.blob_size != 0)
    {
        dfu_updater_nvm_info.blob_size = 0;
        ftl_save(&dfu_updater_nvm_info.blob_size,
                 DFU_UPDATER_NVM_OFFSET + MEMBER_OFFSET(dfu_updater_nvm_info_t, blob_size), 4);
    }
}
#endif

struct
{
    uint16_t image_id;
} dfu_updater_app_ctx;


#if DFU_UPDATER_SUPPORT_POWER_OFF_GO_ON
void dfu_updater_load(void)
{
    if (UNPROV_DEVICE == mesh_node_state_restore())
    {
        dfu_updater_nvm_clear();
    }
    else
    {
        dfu_updater_nvm_load();
        if (dfu_updater_nvm_info.blob_size != 0)
        {
            dfu_updater_app_ctx.image_id = dfu_updater_nvm_info.image_id;
            if (fw_update_server_load(NULL, 0, 0, dfu_updater_nvm_info.transfer_ttl,
                                      dfu_updater_nvm_info.transfer_timeout_base, dfu_updater_nvm_info.blob_id))
            {
                blob_transfer_server_load(dfu_updater_nvm_info.blob_size, dfu_updater_nvm_info.block_size,
                                          dfu_updater_nvm_info.current_block_num,
                                          dfu_updater_nvm_info.transfer_mode, dfu_updater_nvm_info.transfer_mtu_size,
                                          dfu_updater_nvm_info.updater_addr,
                                          dfu_updater_nvm_info.updater_app_key_index);
            }
        }
    }
}

void dfu_updater_clear(void)
{
    dfu_updater_nvm_clear();
    fw_update_server_clear();
    blob_transfer_server_clear();
}
#endif

static int32_t dfu_update_server_data(const mesh_model_info_p pmodel_info, uint32_t type,
                                      void *pargs)
{
    switch (type)
    {
    case FW_UPDATE_SERVER_METADATA_CHECK:
        {
            //fw_update_server_metadata_check_t *pdata = (fw_update_server_metadata_check_t *)pargs;
        }
        break;
    case FW_UPDATE_SERVER_START:
        {
            //fw_update_server_start_t *pdata = (fw_update_server_start_t *)pargs;

            /* change callback since Role change to Updater */
            blob_transfer_server_set_data_cb(fw_update_handle_blob_server_data);
#if DFU_UPDATER_SUPPORT_POWER_OFF_GO_ON
            memcpy(dfu_updater_nvm_info.blob_id, blob_transfer_server_ctx.blob_id, 8);
            dfu_updater_nvm_info.blob_size = blob_transfer_server_ctx.blob_size;
            dfu_updater_nvm_info.block_size = blob_transfer_server_ctx.block_size;
            dfu_updater_nvm_info.current_block_num = 0;
            dfu_updater_nvm_info.transfer_timeout_base = blob_transfer_server_ctx.transfer_timeout_base;
            dfu_updater_nvm_info.transfer_ttl = blob_transfer_server_ctx.transfer_ttl;
            dfu_updater_nvm_info.transfer_mtu_size = blob_transfer_server_ctx.transfer_mtu_size;
            dfu_updater_nvm_info.updater_addr = blob_transfer_server_ctx.updater_addr;
            dfu_updater_nvm_info.updater_app_key_index = blob_transfer_server_ctx.updater_app_key_index;
            dfu_updater_nvm_info.transfer_mode = blob_transfer_server_ctx.mode;
            ftl_save(&dfu_updater_nvm_info, DFU_UPDATER_NVM_OFFSET, sizeof(dfu_updater_nvm_info));
#endif
        }
        break;
    case FW_UPDATE_SERVER_VERIFY:
        {
            bool check_result = silent_dfu_check_checksum(dfu_updater_app_ctx.image_id);
#if DFU_UPDATER_SUPPORT_POWER_OFF_GO_ON
            dfu_updater_nvm_clear();
#endif
            if (false == check_result)
            {
                printw("dfu_update_server_data: verify fail!");
            }
            else
            {
                printi("dfu_update_server_data: update success, verify pass!");
                unlock_flash_all();
            }
            fw_update_server_set_verify_result(check_result);
        }
        break;
    case FW_UPDATE_SERVER_VERIFY_CANCEL:
        break;
    case FW_UPDATE_SERVER_APPLY:
        {
            T_IMG_CTRL_HEADER_FORMAT *p_header = NULL;
            p_header = (T_IMG_CTRL_HEADER_FORMAT *)get_temp_ota_bank_addr_by_img_id((
                                                                                        T_IMG_ID)dfu_updater_app_ctx.image_id);
            dfu_set_image_ready(p_header);
            mesh_reboot(MESH_OTA, 1000);
        }
        break;
    case FW_UPDATE_SERVER_BLOCK_DATA:
        {
            fw_update_server_block_data_t *pdata = (fw_update_server_block_data_t *)pargs;
            if (pdata->block_num == 0)
            {
                T_IMG_HEADER_FORMAT *pimage_header = (T_IMG_HEADER_FORMAT *)pdata->pdata;
                dfu_updater_app_ctx.image_id = pimage_header->ctrl_header.image_id;
#if DFU_UPDATER_SUPPORT_POWER_OFF_GO_ON
                dfu_updater_nvm_info.current_block_num = 1;
                dfu_updater_nvm_info.image_id = dfu_updater_app_ctx.image_id;
                ftl_save(&dfu_updater_nvm_info, DFU_UPDATER_NVM_OFFSET, sizeof(dfu_updater_nvm_info));
#endif
            }
            else
            {
#if DFU_UPDATER_SUPPORT_POWER_OFF_GO_ON
                dfu_updater_nvm_info.current_block_num = pdata->block_num + 1;
                ftl_save(&dfu_updater_nvm_info.current_block_num,
                         DFU_UPDATER_NVM_OFFSET + MEMBER_OFFSET(dfu_updater_nvm_info_t, current_block_num), 4);
#endif
            }
            unlock_flash_all();
            if (sil_dfu_update(dfu_updater_app_ctx.image_id,
                               pdata->block_num * blob_transfer_server_ctx.block_size,
                               pdata->data_len,
                               (uint32_t *)pdata->pdata) != 0)
            {
                /* something wrong */
                printe("dfu_update_server_data: save block data failed");
            }
            lock_flash();
            printi("dfu_update_server_data: %d%%",
                   (pdata->block_num + 1) * 100 / blob_transfer_server_ctx.total_blocks);
        }
        break;
    case FW_UPDATE_SERVER_FAIL:
        {
            fw_update_server_fail_t *pdata = (fw_update_server_fail_t *)pargs;
            printi("dfu_update_server_data: image index %d, reason %d", pdata->fw_image_index, pdata->reason);
#if DFU_UPDATER_SUPPORT_POWER_OFF_GO_ON
            dfu_updater_nvm_clear();
#endif
        }
        break;
    default:
        break;
    }

    return MODEL_SUCCESS;
}

void dfu_updater_models_init(uint8_t element_index)
{
    fw_update_server_reg(element_index, dfu_update_server_data);
    blob_transfer_server_reg(element_index, fw_update_handle_blob_server_data);

    fw_id_t fw_id;
    fw_id.company_id = 0x005d;
    fw_id.version[0] = 0x00;
    fw_id.version[1] = 0x01;
    fw_update_server_add_info(&fw_id, 4, NULL, 0);
}

#endif /* F_BT_MESH_1_1_DFU_SUPPORT */
