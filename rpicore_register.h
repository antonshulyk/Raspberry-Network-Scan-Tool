#ifndef RPICORE_REGISTER_H
#define RPICORE_REGISTER_H

#include "rpicore_global.h"

#define  SERVER_URL_PATH_FOR_REGISTER		"http://192.168.126.158/register"
#define	 REGISTER_PATH			"/var/log/rpicore"


struct rpi_register
{
	char result_val[MAX_RESULT_LEN + 1];                            /* result value for registration */
	char err_message[MAX_PATH + 1];					/* error message for registration */
	int  err_code;							/* error code for registration */
};

/*
 * rpicore server list manager structure
 */

struct rpicore_register_mgr
{
	/* paths for update list */
	char register_dir_path[MAX_PATH];
	char register_fpath[MAX_PATH];
	char register_bundle_fpath[MAX_PATH];
	char rpicore_uuid[MAX_RPICORE_UUID_LEN];
	
	unsigned int curl_err_code;                       /* current error code related with curl operation */
	
	struct rpi_register rpi_register;                /* server list to update */
	
	struct rpicore_context *c;                     /* rpicore context pointer */
};

/*
 * Update List manager APIs
*/

int
rpicore_register_mgr_init(struct rpicore_context *c);

void
rpicore_register_mgr_finalize(struct rpicore_register_mgr *uptlist_mgr);

#endif // RPICORE_UPTLIST_H
