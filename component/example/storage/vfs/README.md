# Example Description

This example is for virtual file system module which wraps littlefs/fatfs.

# HW Configuration

None

# SW configuration

1. In `/ameba<test board>/project_<core>/src/main.c` `app_filesystem_init` interface CONFIG your expected test mode.  
Test Mode: (`VFS_LITTLEFS`/`VFS_FATFS`, `VFS_INF_FLASH`/`VFS_INF_SD`)
   - For `VFS_LITTLEFS` with `VFS_INF_FLASH`:
		```C
		ret = vfs_user_register("lfs", VFS_LITTLEFS, VFS_INF_FLASH, VFS_FLASH_R1, VFS_RW);
		```

   - For `VFS_FATFS` with `VFS_INF_FLASH`:
		```C
		ret = vfs_user_register("fatfs", VFS_FATFS, VFS_INF_FLASH, VFS_FLASH_R1, VFS_RW);
		```

   - For `VFS_FATFS` with `VFS_INF_SD`: (Only AmebaSmart support)
		```C
		ret = vfs_user_register("fatfs", VFS_FATFS, VFS_INF_SD, VFS_FLASH_R1, VFS_RW);
		```

2. Type command `./menuconfig.py` and choose `CONFIG VFS`
   - For `VFS_LITTLEFS`:
		```
		[*] Enable VFS LITTLEFS
		[ ] Enable VFS FATFS
		```

   - For `VFS_FATFS` with `VFS_INF_FLASH`:
		```
		[ ] Enable VFS LITTLEFS
		[*] Enable VFS FATFS
		FATFS Memory Type (FlASH)
		```

   - For `VFS_FATFS` with `VFS_INF_SD`: (Only AmebaSmart support)
		```
		[ ] Enable VFS LITTLEFS
		[*] Enable VFS FATFS
		FATFS Memory Type (SD)
		```

3. How to use:
   - Run `./build.py -a vfs` under project path, e.g. `amebasmart_gcc_project/`, to generate images.
   - `Download` images to board by Ameba Image Tool.

# Expect result

1. Write vfs file (file_name = vfs_key, value = VFS module works normally!!!)

2. Read vfs file

3. Delete vfs file

# Note

None

# Supported IC

- AmebaSmart
- AmebaLite
- AmebaDplus
