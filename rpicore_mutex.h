#ifndef RPICORE_MUTEX_H
#define RPICORE_MUTEX_H

/*
 * rpicore mutex object
 */

struct rpicore_mutex
{
	pthread_mutex_t mt;
};

/*
 * rpicore mutex manipulation APIs
 */

void
rpicore_mutex_init(struct rpicore_mutex *mutex);

void
rpicore_mutex_destroy(struct rpicore_mutex *mutex);

void
rpicore_mutex_lock(struct rpicore_mutex *mutex);

void
rpicore_mutex_unlock(struct rpicore_mutex *mutex);
	
#endif
