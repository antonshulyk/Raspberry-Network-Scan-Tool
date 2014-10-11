
#include "syshead3.h"

#include "rpicore_main.h"

#include "rpicore_curl.h"
#include "rpicore_debug.h"

#include "rpicore_uptlist.h"


#define NBTSCAN_KEYWORD		"------------------------------------------------------------------------------"

/*
 * free all report
*/

static void
free_all_report(struct rpicore_report *report)
{
	/* initialize report */
	memset(report, 0, sizeof(struct rpicore_report));
}

int json_encoder (const char *result_path)
{

    return 0;
}



/*
 * update report from json parser
 */

static int
update_report_from_json(json_object *j_obj, struct rpicore_report *rpi_report)
{
	json_object *j_sub_obj;					/* json object for sub items */
	const char *json_str;						/* string for json object */
	
	char result_val[MAX_RESULT_LEN + 1];					/* get result value from web service */

	int arraylen, is_success, i;

	j_sub_obj = json_object_object_get(j_obj, "success");
    	if (!j_sub_obj)
        {
		rpicore_debug_err(2, "RPICORE Report : Parsing 'success' JSON field has failed\n");
		return -1;
	}
	else
	{
	    is_success = json_object_get_int(j_sub_obj);
	    
	    if (is_success == 0)
    	    {
		rpicore_debug_err(2, "RPICORE Report : The result for report has failed\n");
	    }
	    else
	    {
		rpicore_debug_msg(2, "RPICORE Report : The result for report has successed\n");
	    }
	}
	j_sub_obj = json_object_object_get(j_obj, "data");
	if (!j_sub_obj || (j_sub_obj && json_object_get_type(j_sub_obj) != json_type_array))
    	{
	    rpicore_debug_err(2, "RPICORE Report: Parsing 'data' JSON field has failed\n");
	    return -1;
	}
    
	/* get array length */
	arraylen = json_object_array_length(j_sub_obj);
	if (arraylen < 1)
	{
	    rpicore_debug_err(2, "RPICORE Report: Parsing 'data' JSON field has failed\n");
	    return -1;
	}

	for (i = 0; i < arraylen; i++)
	{
	    json_object *j_arr_obj;
	    json_object *j_sub_arr_obj;
	
	    j_arr_obj = json_object_array_get_idx(j_sub_obj, i);
	    
	    if (j_arr_obj)
    	    {
		json_object *job_obj;
		
		memset (result_val, 0, sizeof(result_val));	    

		/* get  name from json */
		j_sub_arr_obj = json_object_object_get(j_arr_obj, "id");
		if (!job_obj)
    		{
		    rpicore_debug_err(2, "RPICORE Report: Parsing 'id' JSON field has failed\n");
		    return -1;
		}else
		{
		    sprintf(result_val, "RPICORE Report: The report result for id:%d is ", json_object_get_int(j_sub_arr_obj));
		}

		j_sub_arr_obj = json_object_object_get(j_arr_obj, "success");
		if (!job_obj)
    		{
		    rpicore_debug_err(2, "RPICORE Report: Parsing 'id' JSON field has failed\n");
		    return -1;
		}
		else
		{
		    if (is_success == 0)
    		    {
			sprintf(result_val, "%sfailed\n", result_val);
		    }
		    else
		    {
			sprintf(result_val, "%ssuccessed\n", result_val);
		    }
		}
	    
		rpicore_debug_msg(2, "%s", result_val);
    	    }
    	}	

	return 0;
}

/*
 * update report from file
 */

static int
update_report_from_file(struct rpicore_report_mgr *report_mgr, const char *file_path)
{
	struct rpicore_report *report = &report_mgr->rpi_report;
	
	FILE *list_fp;			/* pointer of temporary file */
	struct stat st;			/* status of temporary file */
	
	char *pbuf = NULL;			/* file buffer */
	
	json_object *j_obj;			/* json object for parsing */
	
	int ret;				/* return value */
	
	rpicore_debug_msg(2, "RPICore Report: Updating report from file '%s'\n", file_path);
	
	/* open temporary file */
	list_fp = fopen(file_path, "r");
	if (!list_fp)
	{
		rpicore_debug_err(2, "RPICore Report: report result from file '%s' has failed. Could not open it\n", file_path);
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
		rpicore_debug_err(2, "RPICore Report: Parsing entire report result JSON string has failed from file '%s'\n", file_path);
		return -1;
	}
	
	/* update report from json */
	ret = update_report_from_json(j_obj, report);
	json_object_put(j_obj);
	
	return ret;
}

/*
 * update report from web service
 */

static int
update_report_from_websrv(struct rpicore_report_mgr *report_mgr)
{
	int ret = 0;					/* return value */
	char *read_line;
	char result_json[MAX_PATH];
	size_t len = 0;
	ssize_t read;
	int i = 0;
	int index = 0;
	float ping_ret;					/* Ping Execution time */
	
	char url[MAX_URL_LEN + 1];
	
	/* set URL */
	memset(url, 0, sizeof(url));

	sprintf(url, "%sapi/report", RPICORE_SERVER_DOMAIN);
	rpicore_debug_msg(2, "RPICore Report: Downloading report from URL %s\n", url);
	
	/* remove endline charactors */
	report_mgr->rpicore_uuid[strlen(report_mgr->rpicore_uuid) - 1] = '\0';
    
	/*Creating a json object*/
	json_object * jobj = json_object_new_object();

	/*Form the json object*/
	json_object_object_add(jobj, "token", json_object_new_string(report_mgr->rpicore_uuid));
	
	json_object *j_sub_obj = json_object_new_object();

	/*Creating a json array*/
	json_object *j_job_arr_obj = json_object_new_array();

	/* get job list for report */
	for (index =0; index < g_list_length(g_job_list); index ++)
	{
	    struct rpicore_cmdlist *p = (struct rpicore_cmdlist*)g_slist_nth(g_job_list, index)->data;
	    json_object *j_arr_sub_obj = json_object_new_object();
	    printf("The last job's name is '%d'\n", p->job_id);
	    json_object_object_add(j_arr_sub_obj, "id", json_object_new_int(p->block_id));
	    json_object_object_add(j_arr_sub_obj, "job", json_object_new_int(p->job_id));
	    json_object_object_add(j_arr_sub_obj, "type", json_object_new_int(p->job_type));
	    json_object_object_add(j_arr_sub_obj, "status", json_object_new_int(p->job_status));
	    json_object_object_add(j_arr_sub_obj, "start", json_object_new_string("Tue 16 Sep 2014 16:37:05"));
	    
	    /* get ping result */	    
	    if (p->job_type == PING)
	    {
		ping_ret = get_ping_result (p->result_val);
		fprintf(stderr, "%d", ping_ret);
		json_object_object_add(j_arr_sub_obj, "result", json_object_new_int(ping_ret));
	    }else if(p->job_type == TRACEROUTE)
	    {
		char *tmp;
		int hop;
		float roundTripTime;
		char tmpreceiverName[64], receiverName[64], receiverIP[64];
		int skipFlag = 0;

		/* meta object */
		json_object *j_meta_obj = json_object_new_array();

		tmp	= p->result_val;

		do 
		{
		    /* meta array */
		    json_object *j_meta_arr_obj = json_object_new_object();
		    /* meta->result object */
		    json_object *j_meta_res_obj = json_object_new_object();
		    
		    if (skipFlag == 0)
		    {
			tmp = strchr(tmp, '\n');
    
			skipFlag = 1;
			continue;
			
		    }
		    if (sscanf(tmp, "%d %s (%s %f ms", &hop, receiverIP, receiverName, &roundTripTime) > 0)
		    {
			if (strcmp(receiverIP, "*") != 0)
			{
			    receiverName[(strlen(receiverName))-1] = 0;

			    json_object_object_add(j_meta_arr_obj, "type", json_object_new_int(1));
			    json_object_object_add(j_meta_arr_obj, "ttl", json_object_new_int(hop));

			    json_object_object_add(j_meta_res_obj, "receiverIP", json_object_new_string(receiverIP));
			    json_object_object_add(j_meta_res_obj, "roundTripTime", json_object_new_int(roundTripTime));
			    json_object_object_add(j_meta_res_obj, "receiverName", json_object_new_string(receiverName));

			    /* add meta->result object */
			    json_object_object_add(j_meta_arr_obj, "result", j_meta_res_obj);
			}
			else
			{
			    json_object_object_add(j_meta_arr_obj, "type", json_object_new_int(2));
			    json_object_object_add(j_meta_arr_obj, "ttl", json_object_new_int(hop));
			}

			json_object_array_add(j_meta_obj, j_meta_arr_obj);
			fprintf(stderr, "%d, %s, %s, %f\n", hop, receiverIP, receiverName, roundTripTime);
		    }
		    
		    tmp = strchr(tmp, '\n');
    
		} while ((tmp++) != NULL);

		json_object_object_add(j_arr_sub_obj, "result", json_object_new_int(45));

		/* add meta object */
		json_object_object_add(j_arr_sub_obj, "metadata", j_meta_obj);
	    
	    }else if(p->job_type == NETSCAN)
	    {
    		char *tmp;
		char	ipAddress[64], biosName[64], macAddress[64], serverName[64], userName[64];
	
		tmp = p->result_val;
		/* meta object */
		json_object *j_meta_obj = json_object_new_array();

    		
		tmp = strstr(tmp, NBTSCAN_KEYWORD);
    		tmp = tmp + (strlen(NBTSCAN_KEYWORD) + 1);
    	
		json_object *j_meta_arr_obj = json_object_new_array();
		
		do 
    		{
		    /* meta->result object */
		    json_object *j_meta_res_obj = json_object_new_object();
		    
		    if (sscanf(tmp, "%s %s %s %s %s", ipAddress, biosName, serverName, userName, macAddress) > 0)
		    {
			    json_object_object_add(j_meta_res_obj, "ip", json_object_new_string(ipAddress));
			    json_object_object_add(j_meta_res_obj, "name", json_object_new_string(biosName));
			    json_object_object_add(j_meta_res_obj, "mac", json_object_new_string(macAddress));

			    /* add meta->result object */
			    json_object_array_add(j_meta_arr_obj, j_meta_res_obj);
			    fprintf(stderr, "%s, %s, %s, %s, %s\n", ipAddress, biosName, serverName, userName, macAddress);
		    }
		    tmp = strchr(tmp, '\n');
    		}while ((tmp++) != NULL);

		json_object_object_add(j_arr_sub_obj, "metadata", j_meta_arr_obj);

		json_object_object_add(j_arr_sub_obj, "result", json_object_new_int(52));
	    }

	    json_object_array_add(j_job_arr_obj, j_arr_sub_obj);
	    
	}
    
	json_object_object_add (j_sub_obj, "jobs", j_job_arr_obj);
	json_object_object_add (jobj, "data", j_sub_obj);

	// build json string to post
	snprintf(result_json, sizeof(result_json), "%s", json_object_to_json_string_ext(jobj, JSON_C_TO_STRING_PLAIN));

	/*Now printing the json object*/
	printf ("The json object created: %s\n",json_object_to_json_string(jobj));
	printf ("Result Json value is %s", result_json);

	ret = rpicore_curl_download(url, result_json, report_mgr->report_fpath, &report_mgr->curl_err_code);
	if (ret < 0)
	{
		rpicore_debug_err(2, "RPICore Report: Downloading report from %s has failed (%s)\n",
						 url, curl_easy_strerror((CURLcode) report_mgr->curl_err_code));
		return -1;
	}
	
	rpicore_debug_msg(2, "RPICore Report: Downloading report has succeeded\n");
	
	/* free json object */
	json_object_put(jobj);

	/* update report from temporary file */
	return update_report_from_file(report_mgr, report_mgr->report_fpath);
}

/*
 * update report
 */

void *
update_report(void *args)
{
	int ret;
	struct rpicore_report_mgr *report_mgr = (struct rpicore_report_mgr *) args;
	
	rpicore_debug_msg(2, "RPICore Report: RPICore update report process started\n");
	
	ret = update_report_from_websrv(report_mgr);
	if (ret == 0)                                           /* update to new list */
    	{
		rpicore_debug_msg(2, "RPICore Report: Update new report from web service succeeded\n");
        }
	else if (ret == 1)                                      /* non-update, i.e, md5sum was matched */
        {
	    rpicore_debug_msg(2, "RPICore Report: MD5sum of report for web service matched\n");
        }

	rpicore_debug_msg(2, "RPICore Report: Update report manager has finished.\n");
	
	return 0;
}

/*
 * initialize report manager
 */
int
rpicore_report_mgr_init(struct rpicore_context *c)
{
	struct rpicore_report_mgr *report_mgr = &c->report_mgr;
	
	memset(report_mgr, 0, sizeof(struct rpicore_report_mgr));

	rpi_strcpy(report_mgr->rpicore_uuid, MAX_RPICORE_UUID_LEN, &c->rpicore_uuid);

	/* create directory for storing report */
	snprintf(report_mgr->report_dir_path, sizeof(report_mgr->report_dir_path), "%s", CMD_LIST_PATH);
	mkdir(report_mgr->report_dir_path, 0755);
	rpicore_debug_msg(2, "RPICore Report: file path is %s\n", report_mgr->report_dir_path);

	snprintf(report_mgr->report_fpath, sizeof(report_mgr->report_fpath), "%s/rpi_report", report_mgr->report_dir_path);
	snprintf(report_mgr->report_bundle_fpath, sizeof(report_mgr->report_bundle_fpath), "%s/rpi_report.bundle", report_mgr->report_dir_path);

	/* init curl error code */
	report_mgr->curl_err_code = CURLE_OK;
	
//	/* init update status */
	report_mgr->update_status = RPICORE_REPORT_STATUS_INIT;
	
	/* create updating report from web service */
	update_report(report_mgr);

	/* set init flag */
	report_mgr->c = c;
	report_mgr->init_flag = 1;
	
	return 0;
}
/*
 * finalize report manager
 */
void
rpicore_report_mgr_finalize(struct rpicore_report_mgr *report_mgr)
{
	rpicore_debug_msg(2, "RPICore Report: Finalize RPICore report manager\n");
	
	/* check for it was inited */
	if (!report_mgr->init_flag)
		return;
	
	/* set init and end flag */
	report_mgr->init_flag = 0;
	report_mgr->end_flag = 1;
	
	/* free report */
	free_all_report(&report_mgr->rpi_report);
}
