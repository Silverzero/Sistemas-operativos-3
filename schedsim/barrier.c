#include "barrier.h"
#include <errno.h>


#ifdef POSIX_BARRIER

/* Wrapper functions to use pthread barriers */

int sys_barrier_init(sys_barrier_t* barrier, unsigned int nthreads) {   
    return pthread_barrier_init(barrier,NULL,nthreads);
}

int sys_barrier_destroy(sys_barrier_t* barrier) {
    return pthread_barrier_destroy(barrier);
}

int sys_barrier_wait(sys_barrier_t *barrier) {
    return pthread_barrier_wait(barrier);
}

#else


/* Inicializacion de la barrera */
int sys_barrier_init(sys_barrier_t *barrier, unsigned int nr_threads)
{
   /* Inicializar campos de la estructura sys_barrier
        ... Completar ....
   */
    barrier->nr_threads_arrived = 0;
    barrier->max_threads = nr_threads;
    pthread_mutex_init(&barrier->mutex, NULL);
    pthread_cond_init(&barrier->cond, NULL);
    
    
    return 0;
}

/* Destruccion de los recursos de la barrera */
int sys_barrier_destroy(sys_barrier_t *barrier)
{
   /* Destruir recursos de sincronizacion asociados a la barrera
        ... Completar ....
   */
    pthread_mutex_destroy(&barrier->mutex);
    pthread_cond_destroy(&barrier->cond);
    
   return 0;
}

/* Operacion de sincronizacion/espera en la barrera */
int sys_barrier_wait(sys_barrier_t *barrier)
{
    /* Idea general implementacion:
       - El hilo que llega a la barrera adquiere el mutex e
        incrementa de forma atomica el contador nr_threads_arrived.
       - Si aun faltan mas hilos por llegar a la barrera el hilo se bloquea en la V.cond.
       - Si es el ultimo hilo en llegar:
            1. resetea el estado de la barrera para el siguiente sys_barrier_wait() y
            2. despierta a todos los hilos bloqueados en la V. cond.
       - Liberar mutex antes de salir de la funcion

        ... Completar ....  
    */
    //adquiere el mutex
    pthread_mutex_lock(&barrier->mutex);
    //incremento el contador
    barrier->nr_threads_arrived++;
    //bloquean los hilos hasta que llegan todos
    while(&barrier->nr_threads_arrived<&barrier->max_threads)
        pthread_cond_wait(&barrier->cond,&barrier->mutex);
    //si es el ultimo hilo resetea el contador
    barrier->nr_threads_arrived = 0;
    //despierta los demas hilos
    pthread_cond_broadcast(&barrier->cond);
    //libera el mutex
    pthread_mutex_unlock(&barrier->mutex);
    return 0;
}

#endif /* POSIX_BARRIER */
