#ifndef RPICORE_UTIL_H
#define RPICORE_UTIL_H

#include "rpicore_global.h"

int
rpi_strcpy(char *dst, size_t max_dst_len, const char *src);

int
rpi_strncpy(char *dst, size_t max_dst_len, const char *src, size_t copy_len);

void
copy_file(const char *old_path, const char *new_path);

int
is_exist_fpath(const char *fpath);

int
run_sys_call (const char *cmd);

// get command output
int
get_cmd_output(const char *cmd, char *out, size_t out_size);

// check network status
int
ping_network(const char *url);

#endif
