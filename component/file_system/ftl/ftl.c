#include "os_wrapper.h"
#include "ameba.h"
#include "basic_types.h"
#include "ftl_nand.h"
#include "ftl_nor.h"
#include "ftl_int.h"


__WEAK uint32_t ftl_save_to_storage(u8 *pdata_tmp, uint16_t offset, uint16_t size)
{
	u32 ret;

#if defined (CONFIG_AMEBASMART)

	if (SYSCFG_BootFromNor()) {
		ret = nor_ftl_save_to_storage(pdata_tmp, offset, size);
	} else {
		ret = nand_ftl_save_to_storage(pdata_tmp, offset, size);
	}
#else
	ret = nor_ftl_save_to_storage(pdata_tmp, offset, size);
#endif

	return ret;
}

__WEAK uint32_t ftl_load_from_storage(void *pdata_tmp, uint16_t offset, uint16_t size)
{
	u32 ret;
#if defined (CONFIG_AMEBASMART)

	if (SYSCFG_BootFromNor()) {
		ret = nor_ftl_load_from_storage(pdata_tmp, offset, size);

	} else {
		ret = nand_ftl_load_from_storage(pdata_tmp, offset, size);
	}
#else
	ret = nor_ftl_load_from_storage(pdata_tmp, offset, size);
#endif

	return ret;
}


uint32_t ftl_init(uint32_t u32PageStartAddr, uint8_t pagenum)
{
	u32 ret;
#if defined (CONFIG_AMEBASMART)

	if (SYSCFG_BootFromNor()) {
		ret = nor_ftl_init(u32PageStartAddr, pagenum);
	} else {
		ret = nand_ftl_init(u32PageStartAddr, pagenum);
	}
#else
	ret = nor_ftl_init(u32PageStartAddr, pagenum);
#endif

	return ret;
}




