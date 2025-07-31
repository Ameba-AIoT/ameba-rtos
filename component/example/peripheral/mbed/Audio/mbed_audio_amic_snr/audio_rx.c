#include "platform_autoconf.h"
#include "platform_stdlib.h"

#include "basic_types.h"
#include "audio_rx.h"

static struct SP_RX_INFO sp_rx_info;
#if defined (CONFIG_AMEBASMART)
static struct SP_RX_INFO_EXT sp_rx_info_ext;
#endif

// static const char* TAG = "AUDIO_RX";

extern volatile u8 record_flag;
volatile u32 pagecnt = 0;

#if defined (CONFIG_AMEBASMART)
extern volatile u8 record_flag_ext;
volatile u32 pagecnt_ext = 0;
#endif

u8 *sp_get_ready_rx_page(void)
{
	struct RX_BLOCK *prx_block = &(sp_rx_info.rx_block[sp_rx_info.rx_usr_cnt]);

	if (prx_block->rx_gdma_own) {
		return NULL;
	} else {
		return (u8 *)prx_block->rx_addr;
	}
}

void sp_read_rx_page(u8 *dst, u32 length)
{
	struct RX_BLOCK *prx_block = &(sp_rx_info.rx_block[sp_rx_info.rx_usr_cnt]);

	DCache_CleanInvalidate(prx_block->rx_addr, length);   //clean before read rx page

	memcpy(dst, (void *)prx_block->rx_addr, length);
	prx_block->rx_gdma_own = 1;
	sp_rx_info.rx_usr_cnt++;
	if (sp_rx_info.rx_usr_cnt == SP_RX_DMA_PAGE_NUM) {
		sp_rx_info.rx_usr_cnt = 0;
	}
}

void sp_release_rx_page(void)
{
	struct RX_BLOCK *prx_block = &(sp_rx_info.rx_block[sp_rx_info.rx_gdma_cnt]);

	if (sp_rx_info.rx_full_flag) {
	} else {
		prx_block->rx_gdma_own = 0;
		sp_rx_info.rx_gdma_cnt++;
		if (sp_rx_info.rx_gdma_cnt == SP_RX_DMA_PAGE_NUM) {
			sp_rx_info.rx_gdma_cnt = 0;
		}
	}
}

u8 *sp_get_free_rx_page(void)
{
	struct RX_BLOCK *prx_block = &(sp_rx_info.rx_block[sp_rx_info.rx_gdma_cnt]);

	if (prx_block->rx_gdma_own) {
		sp_rx_info.rx_full_flag = 0;
		return (u8 *)prx_block->rx_addr;
	} else {
		sp_rx_info.rx_full_flag = 1;
		return (u8 *)sp_rx_info.rx_full_block.rx_addr;	//for audio buffer full case
	}
}

u32 sp_get_free_rx_length(void)
{
	struct RX_BLOCK *prx_block = &(sp_rx_info.rx_block[sp_rx_info.rx_gdma_cnt]);

	if (sp_rx_info.rx_full_flag) {
		return sp_rx_info.rx_full_block.rx_length;
	} else {
		return prx_block->rx_length;
	}
}

u32 sp_rx_complete(void *Data)
{
	struct SP_GDMA_STRUCT *gs = (struct SP_GDMA_STRUCT *)Data;
	PGDMA_InitTypeDef GDMA_InitStruct;
	u32 rx_addr;
	u32 rx_length;

	GDMA_InitStruct = &(gs->SpRxGdmaInitStruct);

	/* Clear Pending ISR */
	GDMA_ClearINT(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum);

	if (pagecnt == 7) {
		GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, DISABLE);
		GDMA_ChnlFree(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum);
		record_flag = 0;

	} else {
		sp_release_rx_page();
		rx_addr = (u32)sp_get_free_rx_page();
		rx_length = sp_get_free_rx_length();
		GDMA_SetDstAddr(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, rx_addr);
		GDMA_SetBlkSize(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, rx_length >> 2);
		GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);
	}

	pagecnt++;
	return 0;
}

void sp_init_rx_variables(u8 *rx_buf, u8 *rx_full_buf)
{
	int i;

	sp_rx_info.rx_full_block.rx_addr = (u32)rx_full_buf;
	sp_rx_info.rx_full_block.rx_length = (u32)SP_FULL_BUF_SIZE;

	sp_rx_info.rx_gdma_cnt = 0;
	sp_rx_info.rx_usr_cnt = 0;
	sp_rx_info.rx_full_flag = 0;

	for (i = 0; i < SP_RX_DMA_PAGE_NUM; i++) {
		sp_rx_info.rx_block[i].rx_gdma_own = 1;
		sp_rx_info.rx_block[i].rx_addr = (u32)(rx_buf + i * SP_RX_DMA_PAGE_SIZE);
		sp_rx_info.rx_block[i].rx_length = SP_RX_DMA_PAGE_SIZE;
	}
}

#if defined (CONFIG_AMEBASMART)
/*EXT*/
u8 *sp_get_ready_rx_page_ext(void)
{
	struct RX_BLOCK_EXT *prx_block_ext = &(sp_rx_info_ext.rx_block_ext[sp_rx_info_ext.rx_usr_cnt_ext]);

	if (prx_block_ext->rx_gdma_own_ext) {
		return NULL;
	} else {
		return (u8 *)prx_block_ext->rx_addr_ext;
	}
}

void sp_read_rx_page_ext(u8 *dst, u32 length)
{
	struct RX_BLOCK_EXT *prx_block_ext = &(sp_rx_info_ext.rx_block_ext[sp_rx_info_ext.rx_usr_cnt_ext]);

	DCache_CleanInvalidate(prx_block_ext->rx_addr_ext, length);   //clean before read rx page

	memcpy(dst, (void *)prx_block_ext->rx_addr_ext, length);
	prx_block_ext->rx_gdma_own_ext = 1;
	sp_rx_info_ext.rx_usr_cnt_ext++;
	if (sp_rx_info_ext.rx_usr_cnt_ext == SP_RX_DMA_PAGE_NUM_EXT) {
		sp_rx_info_ext.rx_usr_cnt_ext = 0;
	}
}

void sp_release_rx_page_ext(void)
{
	struct RX_BLOCK_EXT *prx_block_ext = &(sp_rx_info_ext.rx_block_ext[sp_rx_info_ext.rx_gdma_cnt_ext]);

	if (sp_rx_info_ext.rx_full_flag_ext) {
	} else {
		prx_block_ext->rx_gdma_own_ext = 0;
		sp_rx_info_ext.rx_gdma_cnt_ext++;
		if (sp_rx_info_ext.rx_gdma_cnt_ext == SP_RX_DMA_PAGE_NUM_EXT) {
			sp_rx_info_ext.rx_gdma_cnt_ext = 0;
		}
	}
}

u8 *sp_get_free_rx_page_ext(void)
{
	struct RX_BLOCK_EXT *prx_block_ext = &(sp_rx_info_ext.rx_block_ext[sp_rx_info_ext.rx_gdma_cnt_ext]);

	if (prx_block_ext->rx_gdma_own_ext) {
		sp_rx_info_ext.rx_full_flag_ext = 0;
		return (u8 *)prx_block_ext->rx_addr_ext;
	} else {
		sp_rx_info_ext.rx_full_flag_ext = 1;
		return (u8 *)sp_rx_info_ext.rx_full_block_ext.rx_addr_ext;	//for audio buffer full case
	}
}

u32 sp_get_free_rx_length_ext(void)
{
	struct RX_BLOCK_EXT *prx_block_ext = &(sp_rx_info_ext.rx_block_ext[sp_rx_info_ext.rx_gdma_cnt_ext]);

	if (sp_rx_info_ext.rx_full_flag_ext) {
		return sp_rx_info_ext.rx_full_block_ext.rx_length_ext;
	} else {
		return prx_block_ext->rx_length_ext;
	}
}

u32 sp_rx_complete_ext(void *Data)
{
	struct SP_GDMA_STRUCT *gs = (struct SP_GDMA_STRUCT *)Data;
	PGDMA_InitTypeDef GDMA_InitStruct_Ext;
	u32 rx_addr_ext;
	u32 rx_length_ext;

	GDMA_InitStruct_Ext = &(gs->SpRxGdmaInitStructExt);

	/* Clear Pending ISR */
	GDMA_ClearINT(GDMA_InitStruct_Ext->GDMA_Index, GDMA_InitStruct_Ext->GDMA_ChNum);

	if (pagecnt_ext == 7) {
		GDMA_Cmd(GDMA_InitStruct_Ext->GDMA_Index, GDMA_InitStruct_Ext->GDMA_ChNum, DISABLE);
		GDMA_ChnlFree(GDMA_InitStruct_Ext->GDMA_Index, GDMA_InitStruct_Ext->GDMA_ChNum);
		record_flag_ext = 0;

	} else {
		sp_release_rx_page_ext();
		rx_addr_ext = (u32)sp_get_free_rx_page_ext();
		rx_length_ext = sp_get_free_rx_length_ext();
		GDMA_SetDstAddr(GDMA_InitStruct_Ext->GDMA_Index, GDMA_InitStruct_Ext->GDMA_ChNum, rx_addr_ext);
		GDMA_SetBlkSize(GDMA_InitStruct_Ext->GDMA_Index, GDMA_InitStruct_Ext->GDMA_ChNum, rx_length_ext >> 2);
		GDMA_Cmd(GDMA_InitStruct_Ext->GDMA_Index, GDMA_InitStruct_Ext->GDMA_ChNum, ENABLE);
	}

	pagecnt_ext++;
	return 0;
}

void sp_init_rx_variables_ext(u8 *rx_buf_ext, u8 *rx_full_buf_ext)
{
	int i;

	sp_rx_info_ext.rx_full_block_ext.rx_addr_ext = (u32)rx_full_buf_ext;
	sp_rx_info_ext.rx_full_block_ext.rx_length_ext = (u32)SP_FULL_BUF_SIZE_EXT;

	sp_rx_info_ext.rx_gdma_cnt_ext = 0;
	sp_rx_info_ext.rx_usr_cnt_ext = 0;
	sp_rx_info_ext.rx_full_flag_ext = 0;

	for (i = 0; i < SP_RX_DMA_PAGE_NUM_EXT; i++) {
		sp_rx_info_ext.rx_block_ext[i].rx_gdma_own_ext = 1;
		sp_rx_info_ext.rx_block_ext[i].rx_addr_ext = (u32)(rx_buf_ext + i * SP_RX_DMA_PAGE_SIZE_EXT);
		sp_rx_info_ext.rx_block_ext[i].rx_length_ext = SP_RX_DMA_PAGE_SIZE_EXT;
	}
}
#endif
