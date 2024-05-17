
//#if defined (CONFIG_AMEBASMART)

#include "os_wrapper.h"
#include "platform_stdlib.h"
#include "basic_types.h"
#include "ftl_nand.h"
#include "flash_api.h"
#include "ameba_soc.h"


u32 nand_ftl_phy_page_start_addr;    /* The start offset of flash pages which is allocated to FTL physical map.															   Users should modify it according to their own memory layout!! */
u8 nand_ftl_phy_page_num; 	   /* The number of physical map pages, default is 63*/
u32 ftl_block_num = 3;

u32 gn_cur_pageID = 0;
u8 g_nand_page_num = 0;
u32 PAGE_STRAT_ADDR;
u32 used_flag = 0;


static u32 FTL_USED_PATTERN[12] = {
	0x48484848,
	0x31313131,
	0x81818181,
	0x48484848,
	0x07070707,
	0x81818181,
	0x10101010,
	0x00220011,
	0x0C0C0C0C,
	0x18181818,
	0x81818181,
	0x18181818,
};
ALIGNMTO(64) rtos_mutex_t nand_ftl_mutex_lock;

struct Page_T *n_pPage = 0;


u32 get_bit_number(u32 HostAddr)
{
	return NAND_ADDR_TO_PAGE_ADDR(HostAddr);
}

static u32 _host_addr_to_nand_addr(u32 HostAddr)
{
	return NAND_ADDR_TO_PAGE_ADDR(HostAddr);
}


// logical_addr is 4 bytes alignment addr
uint32_t nand_ftl_check_logical_addr(uint16_t logical_addr)
{
	if (logical_addr & 0x3) {
		FTL_ASSERT(0);
		return NAND_FTL_ERROR_INVALID_LOGICAL_ADDR;
	}

	if (logical_addr >= NAND_MAX_LGSIZE) {
		NAND_FTL_PRINTF(NAND_FTL_LEVEL_ERROR, "ftl_check_logical_addr logical_addr exceed MAX_logical_address_size (0x%x, 0x%x)",
						logical_addr,
						NAND_MAX_LGSIZE);
		return NAND_FTL_ERROR_OUT_OF_SPACE;
	}

	return NAND_FTL_SUCCESS;
}

u32 ftl_nand_read_page(u32 pageaddr, Sectioninfo_Def *pdata)
{
	u32 ByteAddr, PageAddr;
	u32 ret;
	u32 i;
	struct single_Page_T *p_page = (struct single_Page_T *)pageaddr;

	for (i = 0; i < Alloc_page_number; i++) {
		ByteAddr = NAND_ADDR_TO_BYTE_ADDR((u32)(p_page + i));
		PageAddr = _host_addr_to_nand_addr((u32)(p_page + i));
		ret = NAND_Page_Read(PageAddr, ByteAddr, (u32)NAND_PAGE_SIZE, (u8 *)&pdata->page_data[i]);
	}

	return ret;
}

u32 ftl_nand_write_page(u32 pageaddr, Sectioninfo_Def *pdata)
{
	u32 ByteAddr, PageAddr;
	u32 ret;
	u8 i;
	struct single_Page_T *p_page = (struct single_Page_T *)pageaddr;
	for (i = 0; i < Alloc_page_number; i++) {
		PageAddr = _host_addr_to_nand_addr((u32)(p_page + i));
		ByteAddr = NAND_ADDR_TO_BYTE_ADDR((u32)(p_page + i));
		ret = NAND_Page_Write(PageAddr, ByteAddr, (u32)NAND_PAGE_SIZE, (u8 *)&pdata->page_data[i]);
	}

	return ret;
}

u32 get_current_page(void)
{
	u32 i, j;
	Pageinfo_Def *pdata_tmp = (Pageinfo_Def *)rtos_mem_malloc(sizeof(Pageinfo_Def));
	u32 ret = 0;

	for (i = 0; i <= g_nand_page_num; i++) {

		ftl_nand_read_page((u32)(n_pPage + (g_nand_page_num - i)), (Sectioninfo_Def *)pdata_tmp);

		for (j = 0; j < 12; j++) {
			if (FTL_USED_PATTERN[j] != (((((u32)pdata_tmp->used[j * 4 + 3]) << 24) & (u32)0xff000000UL) | \
										((((u32)pdata_tmp->used[j * 4 + 2]) << 16) & (u32)0x00ff0000UL) | \
										((((u32)pdata_tmp->used[j * 4 + 1]) << 8) & (u32)0x0000ff00UL) | \
										((((u32)pdata_tmp->used[j * 4]) << 0) & (u32)0x000000ffUL))) {
				break;
			}
		}

		if (j == 12) {
			used_flag = 1;
			ret = g_nand_page_num - i;
			break;

		}
	}

	rtos_mem_free(pdata_tmp);

	return ret;
}

u32 ftl_check_boundary(uint16_t logical_addr, uint32_t len)
{
	if ((logical_addr + len) > NAND_MAX_LGSIZE) {
		return NAND_FTL_BOUNDARY_ERROR;

	} else {
		return NAND_FTL_STATUS_OK;
	}
}

void ftl_flag_bit_set(u8 *flag_buf, uint16_t logical_addr, uint16_t size)
{
	int i;

	for (i = logical_addr; i < logical_addr + size; i += 4) {
		flag_buf[i / 4 / 8] &= ~BIT(i / 4 % 8);
	}
}

u32 ftl_flag_bit_check(u8 *flag_buf, uint16_t logical_addr, uint16_t size)
{
	int i;

	for (i = logical_addr; i < logical_addr + size; i += 4) {
		if (flag_buf[i / 4 / 8] &BIT(i / 4 % 8)) {
			return NAND_FTL_READ_ERROR_READ_NOT_FOUND;
		}
	}
	return 0;
}

__WEAK uint32_t nand_ftl_save_to_storage_i(void *pbuffer, uint16_t logical_addr, uint16_t size)
{
	uint32_t ret = NAND_FTL_WRITE_SUCCESS;

	if (nand_ftl_check_logical_addr(logical_addr)) {
		FTL_ASSERT(0);
		return NAND_FTL_WRITE_ERROR_INVALID_ADDR;
	}
	if (ftl_check_boundary(logical_addr, size)) {
		FTL_ASSERT(0);
		return NAND_FTL_BOUNDARY_ERROR;
	}

	if (g_nand_page_num == 0) {
		return NAND_FTL_ERROR_NOT_INIT;
	}

	Pageinfo_Def *pdata_tmp = (Pageinfo_Def *)malloc(sizeof(Pageinfo_Def));
	memset(pdata_tmp, 0xFF, NAND_LOGPAGE_SIZE);
	ret = ftl_nand_read_page((u32)(n_pPage + gn_cur_pageID), (Sectioninfo_Def *)pdata_tmp);

	if (ret != 0) {
		goto Return_try;
	} else {
		memcpy(&pdata_tmp->payload[logical_addr], pbuffer, size);
		ftl_flag_bit_set(pdata_tmp->bitflag, logical_addr, size);
		memcpy(&pdata_tmp->used, FTL_USED_PATTERN, PATTERN_SIZE);
	}

	if ((++gn_cur_pageID > g_nand_page_num) || (used_flag == 0)) {
		gn_cur_pageID = 0;
	}

	ret = ftl_nand_write_page((u32)(n_pPage + gn_cur_pageID), (Sectioninfo_Def *)pdata_tmp);

	if (ret) {
		goto Return_try;
	}

	if ((gn_cur_pageID & (BLOCK_SHIFT - 1)) == 0) {
		u32 old_block_id = GET_PRE_BLOCK_ID(gn_cur_pageID);
		ret = NAND_Erase(_host_addr_to_nand_addr((u32)(n_pPage + old_block_id * BLOCK_SHIFT)));
		if (ret) {
			goto Return_try;
		}
	}

	if (used_flag == 0) {
		used_flag = 1;
	}
Return_try:
	free(pdata_tmp);
	return ret;
}

__WEAK uint32_t nand_ftl_save_to_storage(void *pbuffer, uint16_t logical_addr, uint16_t size)
{
	u32 ret;
	if (nand_ftl_mutex_lock == NULL) {
		return NAND_FTL_ERROR_NOT_INIT;
	} else if (rtos_mutex_take(nand_ftl_mutex_lock, 100) != 0) {
		return NAND_FTL_ERROR_MUTEX_GET_TIMEOUT;
	}

	ret = nand_ftl_save_to_storage_i(pbuffer, logical_addr, size);

	rtos_mutex_give(nand_ftl_mutex_lock);
	return ret;
}

__WEAK uint32_t nand_ftl_load_from_storage_i(void *pbuffer, uint16_t logical_addr, uint16_t size)
{
	uint32_t ret = NAND_FTL_READ_SUCCESS;

	if (nand_ftl_check_logical_addr(logical_addr)) {
		return NAND_FTL_READ_ERROR_INVALID_LOGICAL_ADDR;
	}

	if (g_nand_page_num == 0) {
		return NAND_FTL_ERROR_NOT_INIT;
	}

	Pageinfo_Def *pdata_tmp = (Pageinfo_Def *)malloc(sizeof(Pageinfo_Def));
	memset(pdata_tmp, 0xFF, NAND_LOGPAGE_SIZE);

	ret = ftl_nand_read_page((u32)(n_pPage + gn_cur_pageID), (Sectioninfo_Def *)pdata_tmp);

	if (ret) {
		goto Return_try;
	} else {
		if (ftl_flag_bit_check((u8 *)&pdata_tmp->bitflag, logical_addr, size)) {
			ret = NAND_FTL_READ_ERROR_READ_NOT_FOUND;
			goto Return_try;

		} else {
			memcpy(pbuffer, &pdata_tmp->payload[logical_addr], size);
		}
	}

Return_try:

	rtos_mem_free(pdata_tmp);

	return ret;
}

__WEAK uint32_t nand_ftl_load_from_storage(void *pbuffer, uint16_t logical_addr, uint16_t size)
{
	u32 ret;

	if (nand_ftl_mutex_lock == NULL) {
		return NAND_FTL_ERROR_NOT_INIT;
	} else if (rtos_mutex_take(nand_ftl_mutex_lock, 100) != 0) {
		return NAND_FTL_ERROR_MUTEX_GET_TIMEOUT;
	}

	ret = nand_ftl_load_from_storage_i(pbuffer, logical_addr, size);

	rtos_mutex_give(nand_ftl_mutex_lock);
	return ret;
}

uint32_t nand_ftl_init(uint32_t u32PageStartAddr, uint8_t pagenum)
{
	rtos_mutex_create(&nand_ftl_mutex_lock);

	g_nand_page_num = pagenum / Alloc_page_number;
	PAGE_STRAT_ADDR = u32PageStartAddr;
	n_pPage = (struct Page_T *)(u32PageStartAddr);
	gn_cur_pageID = get_current_page();

	return 0;
}

//#endif
