#ifndef _VFS_H
#define _VFS_H
#ifdef __cplusplus
extern "C" {
#endif

#include "basic_types.h"
#include "log.h"
#include <sys/unistd.h>
#include <sys/stat.h>

#define VFS_PATH_MAX 256

#define VFS_O_ACCMODE	0x00000003
#define VFS_O_RDONLY	0x00000000
#define VFS_O_WRONLY	0x00000001
#define VFS_O_RDWR		0x00000002
#define VFS_O_CREAT		0x00000100
#define VFS_O_EXCL		0X00000200
#define VFS_O_TRUNC		0x00001000
#define VFS_O_APPEND	0x00002000
#define VFS_O_CLOEXEC	0x02000000

/*vfs_type*/
#define VFS_FATFS		0X00
#define VFS_LITTLEFS	0X01
#define VFS_FS_MAX	0x02

/*vfs_interface_type*/
#define VFS_INF_FLASH			0x00
#define VFS_INF_SECOND_FLASH	0x01
#define VFS_INF_SD				0x02
#define VFS_INF_SD_SPI			0x03
#define VFS_INF_USBH				0x04

/*vfs read-write permission when initializing*/
#define VFS_RW 0x00
#define VFS_RO 0x01

#define VFS_REGION_1	0x00	//flash layout vfs1 region
#define VFS_REGION_2	0x01	//flash layout vfs2 region
#define VFS_REGION_3	0x02	//fatfs with app image region
#define VFS_REGION_4	0x03	//second flash , SD card or SPI-compatible SD NAND
#define VFS_USER_REGION_MAX 0x04		//number of supported file system regions

/* related to op_prepend_header.py */
#define PATTERN_VFS_1 0x5f736676
#define PATTERN_VFS_2 0x5f746166

#define VFS_PREFIX "vfs"
#define VFS_R3_PREFIX "fat1"

#define DT_DIR 0x04
#define DT_REG 0x08

enum {
	VFS_ERROR = 0,
	VFS_WARNING,
	VFS_INFO,
	VFS_DEBUG,
	VFS_NONE,
};

#define VFS_DBG_LEVEL VFS_ERROR

#define VFS_DBG_ON	0
#if VFS_DBG_ON
#define VFS_DBG(level, fmt, arg...)     \
do {\
	if (level <= VFS_DBG_LEVEL) {\
		if (level <= VFS_ERROR) {\
			RTK_LOGS(NOTAG, RTK_LOG_ERROR, "\n\r[error] %s, " fmt "\n\r", __func__, ##arg);\
		} \
		else if(level == VFS_WARNING){\
			RTK_LOGS(NOTAG, RTK_LOG_WARN, "[warning] %s, " fmt "\n", __func__, ##arg);\
		} \
		else if(level == VFS_INFO){\
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "[info] %s, " fmt "\n", __func__, ##arg);\
		} \
		else if(level == VFS_DEBUG){\
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "[debug] %s, " fmt "\n", __func__, ##arg);\
		} \
	}\
}while(0)
#else
#define VFS_DBG(level, fmt, arg...)
#endif

#if defined(CONFIG_AMEBASMART)
#define SHOULD_USE_NAND() (!SYSCFG_BootFromNor())
#elif defined(CONFIG_AMEBAGREEN2) || defined(CONFIG_AMEBAL2) || defined(CONFIG_AMEBAPRO3)
#define SHOULD_USE_NAND() (!SYSCFG_OTP_BootFromNor())
#endif

struct dirent {
	/* Always zero */
	long d_ino;

	/* File position within stream */
	long d_off;

	/* Structure size */
	unsigned int d_reclen;

	/* Length of name without \0 */
	size_t d_namlen;

	/* File type */
	int d_type;

	/* File name */
	char d_name[VFS_PATH_MAX + 1];
};
typedef struct dirent dirent;

typedef struct _vfs_file {
	int vfs_id;
	unsigned char user_id;
	void *file;
	char name[VFS_PATH_MAX + 1];
} vfs_file;

typedef struct {
	int (*open)(void *fs, const char *filename, const char *mode, vfs_file *finfo);
	int (*read)(void *fs, unsigned char *buf, unsigned int size, unsigned int count, vfs_file *file);
	int (*write)(void *fs, unsigned char *buf, unsigned int size, unsigned int count, vfs_file *file);
	int (*close)(void *fs, vfs_file *file);
	int (*seek)(void *fs, long int offset, int origin, vfs_file *file);
	void (*rewind)(void *fs, vfs_file *file);
	int (*fgetpos)(void *fs, vfs_file *file);
	int (*fsetpos)(void *fs, unsigned int offset, vfs_file *file);
	int (*fflush)(void *fs, vfs_file *file);
	int (*remove)(void *fs, const char *filename);
	int (*rename)(void *fs, const char *old_name, const char *new_name);
	int (*eof)(void *fs, vfs_file *file);
	int (*error)(vfs_file *file); //ferror
	int (*tell)(void *fs, vfs_file *file);
	int (*ftruncate)(void *fs, vfs_file *file, off_t length);
	int (*opendir)(void *fs, const char *name, vfs_file *file);
	struct dirent *(*readdir)(void *fs, vfs_file *file);
	int (*closedir)(void *fs, vfs_file *file);
	int (*rmdir)(void *fs, const char *path);
	int (*mkdir)(void *fs, const char *pathname);
	int (*access)(void *fs, const char *pathname, int mode);
	int (*stat)(void *fs, char *path, struct stat *buf);
	int (*mount)(int interface);
	int (*unmount)(int interface);
	int (*get_interface)(int interface);
	const char	*TAG;
	unsigned char drv_num;
	int vfs_type;
} vfs_opt;

typedef int (*vfs_enc_callback_t)(unsigned char *input, unsigned char *output, unsigned int len);
typedef int (*vfs_dec_callback_t)(unsigned char *input, unsigned char *output, unsigned int len);

typedef struct {
	int vfs_type;
	int vfs_type_id;		//used to match vfs.drv index
	int vfs_interface_type;
	char vfs_ro_flag;		//0:rw; 1: read only
	char vfs_region;
	const char *tag;
	vfs_enc_callback_t vfs_enc_callback;
	vfs_dec_callback_t vfs_dec_callback;
	unsigned char enc_iv_len;
	void *fs;
} user_config;

typedef struct {
	unsigned int drv_num;
	unsigned int user_num;
	vfs_opt	*drv[VFS_FS_MAX];
	user_config user[VFS_USER_REGION_MAX];
} vfs_drv;

extern vfs_drv vfs;
extern vfs_opt fatfs_drv;
extern vfs_opt littlefs_drv;
extern int lfs_mount_flag;
extern int lfs2_mount_flag;
extern int fatfs_mount_flag;
extern int fatfs2_mount_flag;

extern u32 vfs_nand_flash_pagesize;
extern u32 vfs_nand_flash_pagenum;

void vfs_init(void);
void vfs_deinit(void);
int vfs_user_register(const char *prefix, int vfs_type, int interface, char region, char flag);
int vfs_user_unregister(const char *prefix, int vfs_type, int interface);
int vfs_scan_vfs(int vfs_type);
int vfs_register(vfs_opt *drv, int vfs_type);
int find_vfs_number(const char *name, int *prefix_len, int *user_id);
int vfs_user_mount(const char *prefix);
char *find_vfs_tag(char region);
int vfs_check_mount_flag(int vfs_type, int vfs_interface_type, char *operation);

void *opendir(const char *name);
struct dirent *readdir(void *dirp);
int closedir(void *dirp);
int scandir(const char *dirp, struct dirent ***namelist,
			int (*filter)(const struct dirent *),
			int (*compar)(const struct dirent **, const struct dirent **));
int rmdir(const char *path);
int mkdir(const char *pathname, mode_t mode);
int access(const char *pathname, int mode);

#ifdef __cplusplus
}
#endif
#endif