#ifndef _AUDIO_RX_H_
#define _AUDIO_RX_H_

#include "ameba_soc.h"

#define SP_RX_DMA_PAGE_SIZE		4096
#define SP_RX_DMA_PAGE_NUM    	8
#define SP_FULL_BUF_SIZE		4096

#define SP_RX_DMA_PAGE_SIZE_EXT		2048
#define SP_RX_DMA_PAGE_NUM_EXT    	8
#define SP_FULL_BUF_SIZE_EXT		2048

struct SP_GDMA_STRUCT {
	GDMA_InitTypeDef       	SpRxGdmaInitStruct;              //Pointer to GDMA_InitTypeDef
	GDMA_InitTypeDef       	SpRxGdmaInitStructExt;           //Pointer to GDMA_InitTypeDef
};

struct RX_BLOCK {
	u8 rx_gdma_own;
	u32 rx_addr;
	u32 rx_length;
};

struct SP_RX_INFO {
	struct RX_BLOCK rx_block[SP_RX_DMA_PAGE_NUM];
	struct RX_BLOCK rx_full_block;
	u8 rx_gdma_cnt;
	u8 rx_usr_cnt;
	u8 rx_full_flag;
};

#if defined (CONFIG_AMEBASMART)

struct RX_BLOCK_EXT {
	u8 rx_gdma_own_ext;
	u32 rx_addr_ext;
	u32 rx_length_ext;
};

struct SP_RX_INFO_EXT {
	struct RX_BLOCK_EXT rx_block_ext[SP_RX_DMA_PAGE_NUM_EXT];
	struct RX_BLOCK_EXT rx_full_block_ext;
	u8 rx_gdma_cnt_ext;
	u8 rx_usr_cnt_ext;
	u8 rx_full_flag_ext;
};

#endif

struct SP_RX_OBJ {

	u32 sp_clock;
	u32 mono_stereo;
	u32 sp_sample_rate;
	u32 codec_sample_rate;
	u32 tdm_mode;
	u32 fifo_num;
	u32 din_sel;
	u32 sp_data_len;
	u32 sp_channel_len;
	u32 codec_data_len;
	u32 codec_channel_len;
};

u8 *sp_get_free_rx_page(void);
u32 sp_get_free_rx_length(void);
u32 sp_rx_complete(void *Data);
void sp_init_rx_variables(u8 *rx_buf, u8 *rx_full_buf);
#if defined (CONFIG_AMEBASMART)
u8 *sp_get_free_rx_page_ext(void);
u32 sp_get_free_rx_length_ext(void);
u32 sp_rx_complete_ext(void *Data);
void sp_init_rx_variables_ext(u8 *rx_buf_ext, u8 *rx_full_buf_ext);

#endif

#endif
