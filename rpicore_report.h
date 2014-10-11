#ifndef RPICORE_REPORT_H
#define RPICORE_REPORT_H

#include "rpicore_global.h"

#define RPICORE_REPORT_STATUS_INIT             0
#define RPICORE_REPORT_STATUS_UPDATED          1
#define RPICORE_REPORT_STATUS_PERSIST          2

struct rpicore_report
{
	char result_val[MAX_RESULT_LEN];                  /* md5sum for server list */
};

/*
 * rpicore server list manager structure
 */

struct rpicore_report_mgr
{
	int init_flag;                                /* indicate for initialize status, 0: no init, 1: inited */
	int end_flag;
	
	/* paths for update list */
	char report_dir_path[MAX_PATH];
	char report_fpath[MAX_PATH];
	char report_bundle_fpath[MAX_PATH];
	char rpicore_uuid[MAX_RPICORE_UUID_LEN];
	
	int update_status;
	
	unsigned int curl_err_code;                       /* current error code related with curl operation */
	
	struct rpicore_report rpi_report;                /* result to report */
	
	struct rpicore_context *c;                     /* rpicore context pointer */
};

/*
 * Update List manager APIs
 */

int
rpicore_report_mgr_init(struct rpicore_context *c);

void
rpicore_report_mgr_finalize(struct rpicore_report_mgr *report_mgr);

#endif // RPICORE_REPORT_H
