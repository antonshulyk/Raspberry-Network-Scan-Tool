
#include "syshead3.h"

#include "rpicore_main.h"

#include "rpicore_curl.h"
#include "rpicore_debug.h"

#include "rpicore_register.h"

/*
 * free all register result value
*/

static void
free_all_register(struct rpi_register *rpi_register)
{
	/* initialize register values */
	memset(rpi_register, 0, sizeof(struct rpi_register));
}

/*
 * get result value for device registration from json parser
 */

static int
update_register_from_json(json_object *j_obj, struct rpi_register *rpi_register)
{
	json_object 	*j_sub_obj;						/* json object for sub items */
	const char 	*json_str;						/* string for json object */
	int		is_success;						/* check if the register has success */	
	int		err_code;						/* result code for registeration */
	char		err_message[MAX_PATH];					/* error messag for registration*/
	char result_val[MAX_RESULT_LEN + 1];					/* get result for registration from web service */
	
	/* get update value */
	j_sub_obj = json_object_object_get(j_obj, "success");
    	if (!j_sub_obj)
        {
		rpicore_debug_err(2, "RPICORE Register : Parsing 'success' JSON field has failed\n");
		return -1;
	}
	else
	{
	    /* initialize rpi register */
	    free_all_register(rpi_register);
	    is_success = json_object_get_int(j_sub_obj);
//	    rpi_strcpy (result_val, MAX_RESULT_LEN, json_str);
	}
	if (!is_success)
	{
	    rpicore_debug_err(2, "RPICORE Register : The register has failed\n");
	    
	    j_sub_obj = json_object_object_get(j_obj, "data");
    	    if (!j_sub_obj)
    	    {
		rpicore_debug_err(2, "RPICORE Register: Parsing 'data' JSON field has failed\n");
		return -1;
	    }
	    else
	    {
		json_str = json_object_object_get(j_sub_obj, "message");
		if (!json_str)
    		{
	    	    rpicore_debug_err(2, "RPICORE Register: Parsing 'message' JSON field has failed\n");
	    	    return -1;
		}
		else 
		{
	    	    json_str = json_object_get_string(json_str);
		    rpi_strcpy(rpi_register->err_message, MAX_PATH, json_str);
		    rpicore_debug_msg(2, "RPICore Register: '%s'\n", rpi_register->err_message);
    		}
		json_str = json_object_object_get(j_sub_obj, "code");
		if (!json_str)
    		{
	    	    rpicore_debug_err(2, "RPICORE Register: Parsing 'code' JSON field has failed\n");
	    	    return -1;
		}
		else 
		{
		    rpi_register->err_code = json_object_get_int(json_str);
	    	    rpicore_debug_msg(2, "RPICore Register: error code is %d\n", rpi_register->err_code);
		}
	    }
	}	
	
	return 0;
}

/*
 * get register result from file
 */

static int
update_register_from_file(struct rpicore_register_mgr *register_mgr, const char *file_path)
{
	struct rpi_register *rpi_register = &register_mgr->rpi_register;
	
	FILE *list_fp;			/* pointer of temporary file */
	struct stat st;			/* status of temporary file */
	
	char *pbuf = NULL;			/* file buffer */
	
	json_object *j_obj;			/* json object for parsing */
	
	int ret;				/* return value */
	
	rpicore_debug_msg(2, "RPICore Register: Updating register result from file '%s'\n", file_path);
	
	/* open temporary file */
	list_fp = fopen(file_path, "r");
	if (!list_fp)
	{
		rpicore_debug_err(2, "RPICore Register: Update register result from file '%s' has failed. Could not open it\n", file_path);
		return -1;
	}
	
	/* get file buffer size and read buffer */
	stat(file_path, &st);
	pbuf = (char *) malloc(st.st_size + 1);
	if (!pbuf)
	{
		fclose(list_fp);
		return -1;
	}
	
	memset(pbuf, 0, st.st_size + 1);
	fread(pbuf, 1, st.st_size, list_fp);
	
	/* close temporary file */
	fclose(list_fp);
	
	/* get json object from buffer */
	j_obj = json_tokener_parse(pbuf);
	
	/* free buffer */
	free(pbuf);
	
	if (!j_obj || is_error(j_obj))
	{
		rpicore_debug_err(2, "RPICore Register: Parsing entire register result JSON string has failed from file '%s'\n", file_path);
		return -1;
	}
	
	/* update register result from json */
	ret = update_register_from_json(j_obj, rpi_register);
	json_object_put(j_obj);
	
	return ret;
}

/*
 * update register result from web service
 */

static int
update_register_from_websrv(struct rpicore_register_mgr *register_mgr)
{
	int ret = 0;					/* return value */
	
	char url[MAX_URL_LEN + 1];
	char result_json[MAX_PATH];
	
	/* set URL */
	memset(url, 0, sizeof(url));
	sprintf(url, "%sapi/register", RPICORE_SERVER_DOMAIN);
	rpicore_debug_msg(2, "RPICore Register: Downloading register result from URL %s\n", url);

	/* remove endline charactors */
	register_mgr->rpicore_uuid[strlen(register_mgr->rpicore_uuid) - 1] = '\0';
    
	/*Creating a json object*/
	json_object * jobj = json_object_new_object();

	json_object *jtoken = json_object_new_string(register_mgr->rpicore_uuid);

	/*Form the json object*/
	json_object_object_add(jobj, "token", jtoken);

	// build json string to post
	snprintf(result_json, sizeof(result_json), "%s", json_object_to_json_string_ext(jobj, JSON_C_TO_STRING_PLAIN));

	/*Now printing the json object*/
	printf ("The json object created: %s\n",json_object_to_json_string(jobj));

	printf ("Result Json value is %s", result_json);
    
	ret = rpicore_curl_download(url, result_json, register_mgr->register_fpath, &register_mgr->curl_err_code);
	if (ret < 0)
	{
		rpicore_debug_err(2, "RPICore Register: Downloading register result from %s has failed (%s)\n",
						 url, curl_easy_strerror((CURLcode) register_mgr->curl_err_code));
		return -1;
	}
	
	rpicore_debug_msg(2, "RPICore Register: Downloading register result has succeeded\n");
	
	/* update register result from temporary file */
	return update_register_from_file(register_mgr, register_mgr->register_fpath);
}

/*
 * update register result
 */

void *
update_register(void *args)
{
	int ret;
	struct rpicore_register_mgr *register_mgr = (struct rpicore_register_mgr *) args;
	time_t last_update_time;                      /* last update attempt time of register result */
	
	rpicore_debug_msg(2, "RPICore Register: RPICore update register manager started\n");
	
	/* init last update time of register result to current time */
	last_update_time = time(NULL);
	
	ret = update_register_from_websrv(register_mgr);
	if (ret == 0)                                           /* update to new list */
        {
		rpicore_debug_msg(2, "RPICore Register: Update new register result from web service succeeded\n");
        }
	else if (ret == 1)                                      /* non-update, i.e, md5sum was matched */
        {
		rpicore_debug_msg(2, "RPICore Register: MD5sum of register result for web service matched\n");
        }
	
	rpicore_debug_msg(2, "RPICore Register: Update register result manager has finished.\n");
	
	return 0;
}

/*
 * initialize register result manager
 */
int
rpicore_register_mgr_init(struct rpicore_context *c)
{
	struct rpicore_register_mgr *register_mgr = &c->register_mgr;
	
	memset(register_mgr, 0, sizeof(struct rpicore_register_mgr));
	
	rpi_strcpy(register_mgr->rpicore_uuid, MAX_RPICORE_UUID_LEN, &c->rpicore_uuid);
	
	/* create directory for storing register result */
	snprintf(register_mgr->register_dir_path, sizeof(register_mgr->register_dir_path), "%s", UPDATE_LIST_PATH);
	mkdir(register_mgr->register_dir_path, 0755);
	rpicore_debug_msg(2, "RPICore Register: file path is %s\n", register_mgr->register_dir_path);

	snprintf(register_mgr->register_fpath, sizeof(register_mgr->register_fpath), "%s/rpi_register", register_mgr->register_dir_path);
	snprintf(register_mgr->register_bundle_fpath, sizeof(register_mgr->register_bundle_fpath), "%s/rpi_register.bundle", register_mgr->register_dir_path);

	/* init curl error code */
	register_mgr->curl_err_code = CURLE_OK;
	
	/* create updating register result from web service */
	update_register(register_mgr);

	/* set init flag */
	register_mgr->c = c;
	
	return 0;
}
/*
 * finalize register result manager
 */
void
rpicore_register_mgr_finalize(struct rpicore_register_mgr *register_mgr)
{
	rpicore_debug_msg(2, "RPICore Register: Finalize RPICore register result manager\n");
	
	/* free register result */
	free_all_register(&register_mgr->rpi_register);
}
