#include <dlfcn.h>
#include <pthread.h>
#include <stdio.h>

static int (*real_pthread_mutex_lock)(pthread_mutex_t *mutex) = nullptr;
// static int (*pthread_mutex_trylock)(pthread_mutex_t *mutex) = nullptr;
// static int (*pthread_mutex_unlock)(pthread_mutex_t *mutex) = nullptr;

/* wrapping write function call */
int pthread_mutex_lock(pthread_mutex_t *mutex)
{
    printf("Locking a mutex");
    
    real_pthread_mutex_lock = (int(*)(pthread_mutex_t*)) dlsym(RTLD_NEXT, "pthread_mutex_lock");
    real_pthread_mutex_lock( mutex );
}