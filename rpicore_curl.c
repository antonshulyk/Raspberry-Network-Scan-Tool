
#include "syshead3.h"

#include "rpicore_main.h"
#include "rpicore_debug.h"

#include "rpicore_curl.h"

static struct rpicore_curl_options *rpi_curl_op = NULL;
 
static int set_content_headers(const char *post_data, struct curl_slist **post_headers)
{
    struct curl_slist *headers = NULL;
    char buf[256];

    // set content length
    snprintf(buf, sizeof(buf), "Content-Length: %d", strlen(post_data));
    headers = curl_slist_append(headers, buf);
    if (!headers)
	return -1;

    // set content type
    headers = curl_slist_append(headers, "Content-Type: application/json");
    if (!headers)
	return -1;

    *post_headers = headers;

    return 0;
}

/*
 * write downloaded data from REST API
 */

int
rpicore_curl_init (struct rpicore_context *c)
{
	if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK)
		return -1;
	
	rpicore_debug_msg(2, "RPICore RestAPI: Initializing CURL library succeeded\n");
	
	rpi_curl_op = &c->rpi_curl_op;
	
	if (c->op.log_level == RPICORE_LOG_LEVEL_VERBOSE)
		rpi_curl_op->curl_debug = 1;
	
	rpi_curl_op->init_status = 1;
		
	return 0;
}


static size_t
write_data(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	size_t wsize;
	
	wsize = fwrite(ptr, size, nmemb, stream);
	fflush(stream);
	
	return wsize;
}

/*
 * perform curl operation and write into given file path
*/

int
rpicore_curl_download(const char *url, const char *post_data, char *file_path, unsigned int *res)
{
	CURL *curl;
	
	FILE *fp;
	
	int ret = 0;
	
	struct curl_slist *post_headers = NULL;
		
	/* initialize curl operation */
	rpicore_debug_msg(2, "RPICore RestAPI: Initializing CURL library succeeded %s\n", file_path);
	curl = curl_easy_init();
	if (!curl)
	{
		return -1;
	}
	
	/* open file path to write */
        fp = fopen(file_path, "w");
	if (!fp)
        {
		curl_easy_cleanup(curl);
		return -1;
	}
	
	
	/* set down URL */
	curl_easy_setopt(curl, CURLOPT_URL, url);
	if (post_data != NULL)
	{	
	    printf ("here is \n");
    	    // set content header
	    if (set_content_headers(post_data, &post_headers) != 0)
	    {
		rpicore_debug_err(2, "CURL: Could not set option for content-type");
		curl_easy_cleanup(curl);

		return -1;
	    }
	    curl_easy_setopt(curl, CURLOPT_HTTPPOST, post_headers);
	    
	    /* set post data */
	    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data);
	    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(post_data));
	}

	/* set write file operation */
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

	/* timeout after 5 second instead of waiting the customary 120 */
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, RPICORE_CURL_TIMEOUT);

	
	/* perform curl operation */
	*res = curl_easy_perform(curl);
	if (*res != CURLE_OK)
	{
		ret = -1;
	}

	/* cleanup curl operation */
	curl_easy_cleanup(curl);
	
	/* close file */
	fclose(fp);
	
	return ret;
}

void
rpicore_curl_finalize()
{
	if (!rpi_curl_op)
		return;
	
	if (!rpi_curl_op->init_status)
		return;
	
	/* cleanup curl env */
	curl_global_cleanup();
	
	return;
}





