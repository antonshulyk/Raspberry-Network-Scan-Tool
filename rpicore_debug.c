
#include "syshead3.h"

#include "rpicore_debug.h"

#define MAX_LOG_MSG_LEN					4096

static struct rpicore_context *ctx = NULL;		/* context object */

/*
 * init debug environment
 */
int
init_debug(struct rpicore_context *c)
{
	char log_dir_path[MAX_PATH], log_fpath[MAX_PATH];
	
	/* create log directory */
	snprintf(log_dir_path, sizeof(log_dir_path), "%s/log", LOG_FILE_PATH);
	mkdir(log_dir_path, 0755);

	/* open log file for writting */
	snprintf(log_fpath, sizeof(log_fpath), "%s/rpicore.log", log_dir_path);
	c->log_fp = fopen(log_fpath, "w");
	if (!c->log_fp)
	{
		fprintf(stderr, "Could not open log file '%s'(errno:%d)\n", log_fpath, errno);
		exit(-1);
	}
	
	/* set logging buffering option */
	setvbuf(c->log_fp, NULL, _IONBF, 0);
	
	/* create mutex for writing */
	rpicore_mutex_init(&c->log_mt);
	
	ctx = c;
	
	return 0;
}

/*
 * print general debug messages
 */

void
rpicore_debug_msg(int log_level, const char *format, ...)
{
	va_list ap;
	char msg[MAX_LOG_MSG_LEN];
	
	time_t tt;
	struct tm *tm;
	char time_str[64];
	
	if (!ctx)
		return;
	
	if (!ctx->log_fp)
		return;
	
	/* lock mutex */
	rpicore_mutex_lock(&ctx->log_mt);
	
	va_start(ap, format);
	vsnprintf(msg, MAX_LOG_MSG_LEN - 1, format, ap);
	msg[MAX_LOG_MSG_LEN - 1] = '\0';
	va_end(ap);
	
	/* get current time */
	time(&tt);
	tm = localtime(&tt);
	
	strftime(time_str, sizeof(time_str), "%a %d %b %Y %T", tm);
	
	/* print message */
	if (ctx->op.log_level == RPICORE_LOG_LEVEL_VERBOSE)
	{
		fprintf(stderr, "%s\tInfo : %s", time_str, msg);
	}
	
	fprintf(ctx->log_fp, "%s\tInfo : %s", time_str, msg);
	fflush(ctx->log_fp);

	/* unlock mutex */
	rpicore_mutex_unlock(&ctx->log_mt);
}

/*
 * print error messages
 */
void
rpicore_debug_err(int log_level, const char *format, ...)
{
	va_list ap;
	char msg[MAX_LOG_MSG_LEN];
	
	time_t tt;
	struct tm *tm;
	char time_str[64];
	
	if (!ctx)
		return;
	
	if (!ctx->log_fp)
		return;
	
	/* lock mutex */
	rpicore_mutex_lock(&ctx->log_mt);
	
	va_start(ap, format);
	vsnprintf(msg, MAX_LOG_MSG_LEN - 1, format, ap);
	msg[MAX_LOG_MSG_LEN - 1] = '\0';
	va_end(ap);
	
	/* get current time */
	time(&tt);
	tm = localtime(&tt);
	
	strftime(time_str, sizeof(time_str), "%a %d %b %Y %T", tm);
	
	/* print message */
	if (ctx->op.log_level == RPICORE_LOG_LEVEL_VERBOSE)
	{
		fprintf(stderr, "%s\tError : %s", time_str, msg);
	}

	fprintf(ctx->log_fp, "%s\tError : %s", time_str, msg);
	fflush(ctx->log_fp);
	
	/* unlock mutex */
	rpicore_mutex_unlock(&ctx->log_mt);
}
