#ifndef MEW_INCLUDE_MEW_OS_FS_H_
#define MEW_INCLUDE_MEW_OS_FS_H_

#include <stdbool.h>
#include <stdint.h>

#include "mew/str.h"

bool mew_fs_exists(const char *path);
bool mew_fs_get_size(const char *path, uintptr_t *size);
bool mew_fs_read_file_to_sb(const char *path, StringBuilder *asb);

#endif // MEW_INCLUDE_MEW_OS_FS_H_
