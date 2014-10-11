#ifndef RPICORE_DEBUG_H
#define RPICORE_DEBUG_H

#include "rpicore_main.h"

enum {
	RPICORE_LOG_LEVEL_SILENT = 1,
	RPICORE_LOG_LEVEL_NORMAL,
	RPICORE_LOG_LEVEL_VERBOSE,
};

int
init_debug(struct rpicore_context *c);

void
rpicore_debug_msg(int log_level, const char *format, ...);

void
rpicore_debug_err(int log_level, const char *format, ...);
	
#endif	// RPICORE_DEBUG_H
