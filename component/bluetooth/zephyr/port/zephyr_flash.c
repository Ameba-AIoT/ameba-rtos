#include <zephyr_flash.h>
#include <flash_api.h>

static uint32_t flash_location = 0;
static uint16_t flash_sec_cnt = 0;

#define ZEPHYR_FLASH_SECTOR_SIZE 0x1000
#define ZEPHYR_FLASH_SECTOR_COUNT flash_sec_cnt
#define ZEPHYR_FLASH_OFFSET flash_location
#define ZEPHYR_FLASH_WRITE_BLOCK_SIZE 4
#define ZEPHYR_FLASH_ERASE_VALUE 0xFF

struct device zephyr_flash_device = {
	.name = "zephyr",
};

struct flash_parameters zephyr_flash_parameters = {
	.write_block_size = ZEPHYR_FLASH_WRITE_BLOCK_SIZE,
	.erase_value = ZEPHYR_FLASH_ERASE_VALUE,
};

void zephyr_get_flash_info(uint32_t *offset, uint16_t *sec_cnt)
{
	u32 ftl_start_addr, ftl_end_addr;

	flash_get_layout_info(FTL, &ftl_start_addr, &ftl_end_addr);
	*offset = (uint32_t)(ftl_start_addr - SPI_FLASH_BASE);
	*sec_cnt = (uint16_t)((ftl_end_addr - ftl_start_addr + 1) / PAGE_SIZE_4K);
}

struct device *flash_get_device(uint32_t *offset, uint16_t *sec_cnt, uint16_t *sec_size)
{
	zephyr_get_flash_info(&flash_location, &flash_sec_cnt);
	*offset = flash_location;
	*sec_cnt = flash_sec_cnt;
	*sec_size = 0x1000;

	return &zephyr_flash_device;
}

int flash_write(const struct device *dev, off_t offset, const void *data, size_t len)
{
	(void)dev;

	flash_stream_write(NULL, (u32)offset, (u32)len, (u8 *)data);
	return 0;
}

int flash_read(const struct device *dev, off_t offset, void *data, size_t len)
{
	(void)dev;

	flash_stream_read(NULL, (u32)offset, (u32)len, (u8 *)data);
	return 0;
}

int flash_erase(const struct device *dev, off_t offset, size_t size)
{
	(void)dev;
	size_t len = 0;

	while(len < size) {
		flash_erase_sector(NULL, offset + len);
		len += ZEPHYR_FLASH_SECTOR_SIZE;
	}

	return 0;
}

const struct flash_parameters *flash_get_parameters(const struct device *dev)
{
	(void)dev;

	return &zephyr_flash_parameters;
}

size_t flash_get_write_block_size(const struct device *dev)
{
	(void)dev;

	return ZEPHYR_FLASH_WRITE_BLOCK_SIZE;
}

int flash_get_page_info_by_offs(const struct device *dev, off_t offset, struct flash_pages_info *info)
{
	(void)dev;

	uint32_t index;

	if (offset < (off_t)ZEPHYR_FLASH_OFFSET)
		return -1;

	index = (offset - ZEPHYR_FLASH_OFFSET) / ZEPHYR_FLASH_SECTOR_SIZE;

	if (index > (uint32_t)(ZEPHYR_FLASH_SECTOR_COUNT - 1))
		return -1;

	info->size = ZEPHYR_FLASH_SECTOR_SIZE;
	info->index = index;
	info->start_offset = index * info->size;

	return 0;
}