
#include "syshead3.h"

#include "rpicore_main.h"

#include "rpicore_debug.h"

int	is_finish = 0;

/*
 * initialize option values
 */
static void
init_options(struct rpicore_options *op)
{
	memset(op, 0, sizeof(struct rpicore_options));
	
	/* init logging options */
	op->log_level = RPICORE_LOG_LEVEL_VERBOSE;
}

/* daemonize process */
static void daemonize()
{
    int pid, sid;

    /* fork new process */
    pid = fork();
    if (pid < 0)
	exit(1);
    
    if (pid > 0)
    {
	/* child created, parent exit */	
	exit(0);
    }
    
    /* set file permission 750 */
    umask(027);

    /* get new process group */
    sid = setsid();
    if (sid < 0)
	exit(0);

    /* close standard I/O, stdin, stdout, stderr */
    close(0);
    close(1);
    close(2);
}

/*
 * initialize rpicore context components
 */

int
rpicore_ctx_init(struct rpicore_context *c)
{

	
	rpicore_debug_msg(1, "RPICORE main: Starting RPI Core daemon(Version:%s)\n", PACKAGE_VERSION);
	
//    	while (!is_finish)
//    	{
//	    if (ping_network("www.google.com") == 0)
//	    {
//		rpicore_debug_err(2, "RPICORE main: Could not connect to internet\n");
//		fprintf (stderr, "Could not connect to internet\n");
//		continue;
//	    }    

	    /* init curl env */
	    if (rpicore_curl_init(c) == -1)
	    {
		rpicore_debug_err(2, "RPICORE main: RPICORE CURL library init has failed\n");
		goto err;
	    }

	    /* init rpicore register list manager */
	    if (rpicore_register_mgr_init(c) == -1)
    	    {
		rpicore_debug_err(2, "RPICORE main: RPICORE register manager init has failed\n");
		goto err;
	    }

	    /* init rpicore command list manager */
	    if (rpicore_cmdlist_mgr_init(c) == -1)
	    {
		rpicore_debug_err(2, "RPICORE main: RPICORE command list manager init has failed\n");
		goto err;
	    }

//	    /* init rpicore update list manager */
//	    if (rpicore_uptlist_mgr_init(c) == -1)
///	    {
//		rpicore_debug_err(2, "RPICORE main: RPICORE update list manager init has failed\n");
//		goto err;
//	    }

	    /* init rpicore report list manager */
	    if (rpicore_report_mgr_init(c) == -1)
	    {
		rpicore_debug_err(2, "RPICORE main: RPICORE report manager init has failed\n");
		goto err;
	    }

//	    sleep (120);
//	}	

	return 0;
	
err:
	rpicore_ctx_finalize(c);
	
	return -1;
}

/*
 * finalize rpicore context components
 */

void
rpicore_ctx_finalize(struct rpicore_context *c)
{
	rpicore_debug_msg(1, "rpicore main: Finalize rpicore components\n");
	
	/* finalize register manager */
	rpicore_register_mgr_finalize(&c->register_mgr);

	/* finalize command list manager */
	rpicore_cmdlist_mgr_finalize(&c->cmdlist_mgr);

	/* finalize rpicore udate list manager */
	rpicore_uptlist_mgr_finalize(&c->uptlist_mgr);

	/* finalize rpicore udate list manager */
	rpicore_report_mgr_finalize(&c->report_mgr);
	
	rpicore_curl_finalize();
	
	/* close log file */
	if (c->log_fp)
	{
		fclose(c->log_fp);
		c->log_fp = NULL;
	}
	
	/* destroy log mutex */
	rpicore_mutex_destroy(&c->log_mt);
	
	return;
}

int main(int argc, char *argv[]) 
{

    struct rpicore_context ctx;
    int ret;

    /* initialize rpicore context */
    memset (&ctx, 0, sizeof(ctx));

    /* ini context options */
    init_options(&ctx.op);

	/* init debug mode */
    init_debug(&ctx);
    
    ret = check_exist_file(UUID_FILE_PATH, 0);
    if (!ret)
    {
        get_cmd_output(UUID_COMMAND, ctx.rpicore_uuid, sizeof(ctx.rpicore_uuid));
    }
    
    get_uuid_from_file (UUID_FILE_PATH, ctx.rpicore_uuid, sizeof(ctx.rpicore_uuid));
    
    
//    daemonize();

    if (rpicore_ctx_init(&ctx) != 0)
    {
        rpicore_debug_err(2, "RPICORE main: Could not initialize RPI client context\n");
        exit(1);
    }

    return TRUE;
}



