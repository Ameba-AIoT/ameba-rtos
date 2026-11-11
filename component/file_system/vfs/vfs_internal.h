#ifndef _VFS_INTERNAL_H_
#define _VFS_INTERNAL_H_

#include "vfs.h"

void vfs_stub_log(void);
int vfs_target_fs(int target);
int vfs_check_mount_flag(int user_id, const char *operation);

#endif
