#ifndef RPICORE_CURL_H
#define RPICORE_CURL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "rpicore_global.h"

#define MAX_DATARESULT_LEN	4096
#define MAX_CMD_LINE		128
#define SCAN_MAX_LEN		3

struct rpicore_curl_options
{
    int init_status;				/* init status of CURL library */
    int curl_debug;				/* CURL debugging flag */
};


int
rpicore_curl_init (struct rpicore_context *c);

int
rpicore_curl_download(const char *url, const char *post_data, char *file_path, unsigned int *res);

void
rpicore_curl_finalize();

#ifdef __cplusplus
}
#endif

#endif
