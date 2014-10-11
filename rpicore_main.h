#ifndef RPICORE_MAIN_H
#define RPICORE_MAIN_H

struct rpicore_options;
struct rpicore_context;

GSList *g_job_list;
	
#include "rpicore_global.h"
	
#include "rpicore_util.h"
#include "rpicore_mutex.h"
#include "rpicore_curl.h"
#include "rpicore_cmdlist.h"
#include "rpicore_uptlist.h"
#include "rpicore_register.h"
#include "rpicore_report.h"


		
/*
 * rpicore options structure
 */

struct rpicore_options
{
	int log_level;
};

/*
 * rpicore context structure
 */

struct rpicore_context
{
	struct rpicore_options op;			/* options */
	
	FILE *log_fp;					/* file pointer to log file */

	char rpicore_uuid[MAX_RPICORE_UUID_LEN];
	
	struct rpicore_mutex log_mt;			/* mutex for writing log messages */
	
	struct rpicore_curl_options rpi_curl_op;
	
	struct rpicore_cmdlist_mgr cmdlist_mgr;		/* command list manager */
	struct rpicore_uptlist_mgr uptlist_mgr;		/* update list manager */
	struct rpicore_register_mgr register_mgr;	/* register manager */
	struct rpicore_report_mgr report_mgr;		/* report manager */
};
	
int
rpicore_ctx_init(struct rpicore_context *c);

void
rpicore_ctx_finalize(struct rpicore_context *c);

#endif // RPICORE_MAIN_H
