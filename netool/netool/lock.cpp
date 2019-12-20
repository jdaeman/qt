#include <pthread.h>

pthread_mutex_t mutex;

int mutex_init()
{
    return pthread_mutex_init(&mutex, NULL);
}
