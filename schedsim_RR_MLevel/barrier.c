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
    return 0;
}

/* Destruccion de los recursos de la barrera */
int sys_barrier_destroy(sys_barrier_t *barrier)
{
   /* Destruir recursos de sincronizacion asociados a la barrera
        ... Completar ....
   */
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
    return 0;
}

#endif /* POSIX_BARRIER */
