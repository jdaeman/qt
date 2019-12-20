#ifndef LOCK_H
#define LOCK_H

#include <pthread.h>

extern pthread_mutex_t mutex;
extern int mutex_init();

#endif // LOCK_H

