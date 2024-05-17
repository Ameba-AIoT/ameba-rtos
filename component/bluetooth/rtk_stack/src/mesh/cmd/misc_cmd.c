/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     misc_cmd.h
  * @brief    Head file for miscellaneous cmd.
  * @details  User command interfaces.
  * @author   bill
  * @date     2017-12-18
  * @version  v1.0
  * *************************************************************************************
  */

/* Add Includes here */
#include "misc_cmd.h"
#include "gap.h"
#include "gap_bondmgr.h"
#include "profile.h"
#include "profile_client.h"

user_cmd_parse_result_t user_cmd_io_capa_set(user_cmd_parse_value_t *pparse_value)
{
    uint8_t ioCap = GAPBOND_IO_CAP_NO_INPUT_NO_OUTPUT;
    if (pparse_value->para_count >= 1)
    {
        switch (pparse_value->dw_parameter[0])
        {
        case 0:
            ioCap = GAPBOND_IO_CAP_NO_INPUT_NO_OUTPUT;
            data_uart_debug("GAPBOND_IO_CAP_NO_INPUT_NO_OUTPUT\r\n");
            break;
        case 1:
            ioCap = GAPBOND_IO_CAP_DISPLAY_ONLY;
            data_uart_debug("GAPBOND_IO_CAP_DISPLAY_ONLY\r\n");
            break;
        case 2:
            ioCap = GAPBOND_IO_CAP_KEYBOARD_ONLY;
            data_uart_debug("GAPBOND_IO_CAP_KEYBOARD_ONLY\r\n");
            break;
        case 3:
            ioCap = GAPBOND_IO_CAP_DISPLAY_YES_NO;
            data_uart_debug("GAPBOND_IO_CAP_DISPLAY_YES_NO\r\n");
            break;
        case 4:
            ioCap = GAPBOND_IO_CAP_KEYBOARD_DISPLAY;
            data_uart_debug("GAPBOND_IO_CAP_KEYBOARD_DISPLAY\r\n");
            break;
        default:
            data_uart_debug("GAPBOND_IO_CAP parameter error!!!\r\n");
            break;
        }
    }
    else
    {
        data_uart_debug("GAPBOND_IO_CAP_NO_INPUT_NO_OUTPUT\r\n");
    }
    GAPBondMgr_SetParameter(GAPBOND_IO_CAPABILITIES, sizeof(uint8_t), &ioCap);
    GAPBondMgr_SetPairable();

    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_security_req(user_cmd_parse_value_t *pparse_value)
{
    data_uart_debug("Security Req\r\n");
    GAPBondMgr_Pair();
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_con_update_req(user_cmd_parse_value_t *pparse_value)
{
    data_uart_debug("Con Update Req\r\n");
    gap_con_params_update();
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_service_discover(user_cmd_parse_value_t *pparse_value)
{
    data_uart_debug("Service Discoverying\r\n");
    clientAPI_AllPrimarySrvDiscovery();
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_relation_discover(user_cmd_parse_value_t *pparse_value)
{
    TRelationDiscReq relationReq;
    relationReq.wStartHandle = (uint16_t)pparse_value->dw_parameter[0];
    relationReq.wEndHandle = (uint16_t)pparse_value->dw_parameter[1];
    if (relationReq.wStartHandle > relationReq.wEndHandle)
    {
        return USER_CMD_RESULT_WRONG_PARAMETER;
    }
    data_uart_debug("Relationship Discoverying\r\n");
    clientAPI_RelationshipDiscovery(AppProcessGeneralClientMsgID, relationReq);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_character_discover(user_cmd_parse_value_t *pparse_value)
{
    TCharDiscReq char_req;
    char_req.wStartHandle = (uint16_t)pparse_value->dw_parameter[0];
    char_req.wEndHandle = (uint16_t)pparse_value->dw_parameter[1];
    if (char_req.wStartHandle > char_req.wEndHandle)
    {
        return USER_CMD_RESULT_WRONG_PARAMETER;
    }
    data_uart_debug("Char Discoverying\r\n");
    clientAPI_AllCharDiscovery(AppProcessGeneralClientMsgID, char_req);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_character_descriptor_discover(
    user_cmd_parse_value_t *pparse_value)
{
    TCharDescriptorDiscReq charDescriptorReq;
    charDescriptorReq.wStartHandle = (uint16_t)pparse_value->dw_parameter[0];
    charDescriptorReq.wEndHandle = (uint16_t)pparse_value->dw_parameter[1];
    if (charDescriptorReq.wStartHandle > charDescriptorReq.wEndHandle)
    {
        return USER_CMD_RESULT_WRONG_PARAMETER;
    }
    data_uart_debug("Descriptor Discoverying\r\n");
    clientAPI_AllCharDescriptorDiscovery(AppProcessGeneralClientMsgID, charDescriptorReq);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_attribute_read(user_cmd_parse_value_t *pparse_value)
{
    TReadHandleReq readHandle;

    data_uart_debug("Attrib Read\r\n");
    readHandle.wHandle = (uint16_t)pparse_value->dw_parameter[0];
    clientAPI_AttribRead(AppProcessGeneralClientMsgID, readHandle);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_attribute_read_by_uuid(user_cmd_parse_value_t *pparse_value)
{
    TReadUUIDReq readUUIDReq;
    readUUIDReq.UUID16 = (uint16_t)pparse_value->dw_parameter[2];
    readUUIDReq.wStartHandle = (uint16_t)pparse_value->dw_parameter[0];
    readUUIDReq.wEndHandle = (uint16_t)pparse_value->dw_parameter[1];
    if (readUUIDReq.wStartHandle > readUUIDReq.wEndHandle)
    {
        return USER_CMD_RESULT_WRONG_PARAMETER;
    }
    data_uart_debug("Attrib Read by UUID\r\n");
    clientAPI_AttribReadUsingUUID(AppProcessGeneralClientMsgID, readUUIDReq);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_tx_power_set(user_cmd_parse_value_t *pparse_value)
{
    /* Indicate which char to be written. */
    TBLE_TX_POWER_INDEX tx_power_index = (TBLE_TX_POWER_INDEX)pparse_value->dw_parameter[0];
    data_uart_debug("Tx Power Set\r\n");
    gap_set_tx_power(tx_power_index);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_rssi_read(user_cmd_parse_value_t *pparse_value)
{
    data_uart_debug("Rssi Read\r\n");
    gap_read_rssi();
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_patch_version_get(user_cmd_parse_value_t *pparse_value)
{
    uint16_t patchVersion;
    blueAPI_ReadPatchVersion(&patchVersion);
    data_uart_debug("Patch Ver Get: version = %d\r\n", patchVersion);
    return USER_CMD_RESULT_OK;
}

user_cmd_parse_result_t user_cmd_channel_class_set(user_cmd_parse_value_t *pparse_value)
{
    uint8_t chanMap[5] = {0};
    uint8_t i;
    for (i = 0; i < 5; i++)
    {
        chanMap[i] = (uint8_t)pparse_value->dw_parameter[i];
    }
    data_uart_debug("Chan Class Set\r\n");
    chanMap[4] = chanMap[4] & 0x1F;
    gap_set_host_channel_class(chanMap);
    return USER_CMD_RESULT_OK;
}
