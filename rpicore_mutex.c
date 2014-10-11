
#include "syshead3.h"

#include "rpicore_mutex.h"

/*
 * initialize rpicore mutex object
 */

void
rpicore_mutex_init(struct rpicore_mutex *mutex)
{
	pthread_mutex_init(&mutex->mt, NULL);
}

/*
 * destroy rpicore mutex object
 */

void
rpicore_mutex_destroy(struct rpicore_mutex *mutex)
{
	pthread_mutex_destroy(&mutex->mt);
}

/*
 * lock rpicore mutex object
 */

void
rpicore_mutex_lock(struct rpicore_mutex *mutex)
{
	pthread_mutex_lock(&mutex->mt);
}

/*
 * unlock rpicore mutex object
 */

void
rpicore_mutex_unlock(struct rpicore_mutex *mutex)
{
	pthread_mutex_unlock(&mutex->mt);
}
