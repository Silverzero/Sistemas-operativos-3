#ifndef  BARRIER_H
#define  BARRIER_H
#include <pthread.h>

#if defined(__APPLE__) && defined(__MACH__)
#undef POSIX_BARRIER    /* MAC OS X does not implement pthread's barriers so use custom implementation instead */
#endif

#ifdef POSIX_BARRIER 
typedef pthread_barrier_t sys_barrier_t;

#else
/* Barrera de sincronizacion */
typedef struct
{
    pthread_mutex_t mutex;  /* Cerrojo de la barrera */
    pthread_cond_t cond;    /* Variable condicional donde se bloquean los hilos */
    int nr_threads_arrived; /* Numero de hilos que han llegado a la barrera */
    int max_threads;        /* Numero de hilos que han de sincronizarse en la barrera */
}sys_barrier_t;
#endif

int sys_barrier_init(sys_barrier_t* barrier, unsigned int nthreads);
int sys_barrier_destroy(sys_barrier_t* barrier);
int sys_barrier_wait(sys_barrier_t *barrier);

#endif // Barrier


