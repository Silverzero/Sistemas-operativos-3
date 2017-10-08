#ifndef SCHED_H
#define SCHED_H

#include <unistd.h> /* For pid_t */
#include "slist.h"
#include <stdio.h>
#include <pthread.h>

#define MAX_EXEC_PHASES 10
#define MAX_TASK_NAME 20
#define MAX_CPUS 8

#define RUNQUEUE_LEVELS 3

#define TF_IDLE_TASK    0x1
#define TF_INSERT_FRONT 0x2

typedef enum {TS_STARTING=0,TS_RUNNABLE,TS_ONPROC,TS_SLEEP,TS_COMPLETED} task_state_t;
typedef enum {CPU_PHASE,IO_PHASE} phase_t;
typedef enum {FALSE,TRUE} bool;

typedef struct {
    phase_t type;       
    unsigned int len;   /* Phase Lenght in ticks */
}exec_phase_t;

typedef struct {
    exec_phase_t phase[MAX_EXEC_PHASES];    /* Vector of execution phases */       
    unsigned int nr_phases;                 /* Phase count */
    unsigned int arrival_time;              /* Simulation step when the task enters the system */
    int cur_phase;                          /* Index to the current executing phase
                                              -1: means that the app has already finished */
}exec_profile_t;
 
typedef struct {
    task_state_t state;
    int cpu;
    int when;
    int how_long;
    list_node_t links;
}sched_log_t;

/* Task descriptor */
typedef struct{
    int task_id;                    /* Internal ID for the task*/
    char task_name[MAX_TASK_NAME];
    exec_profile_t task_profile; /* Task behavior */
    int prio;
    task_state_t state; 
    int last_cpu;               /* CPU where the task ran last time */
    int last_time_enqueued;     /* Last simulation step where the task was enqueued */
    int runnable_ticks_left;    /* Number of ticks the application has to complete till blocking or exiting */
    int insert_level;           /* Next level in the rqs */

    int remaining_ticks_slice;  /* For the RR scheduler */
    list_node_t ts_links;        /* Node for the global task list */
    list_node_t rq_links;	/* Node for the RQ list */
    bool on_rq;                 /* Marker to check if the task is on the rq or not !! */
    unsigned long flags;        /* generic flags field */
/* Global statistics */
    int user_time;  		/* Cpu time */
    int real_time;  		/* Elapsed time since the application entered the system */
    int sys_time;   		/* For now this time reflects the time the thread spends doing IO */
    slist_t sched_regs; 	/* Linked list to keep track of the sched log registers (track state changes for later use) */
}task_t;

/* Descriptor for a per-CPU run queue */
typedef struct{
    //slist_t tasks;      	/* runnable task queue */
    slist_t level_tasks[RUNQUEUE_LEVELS];     /* runnable task queue */
    task_t* cur_task;   	/* Pointer to the task in the CPU. It may be the idle task*/
    task_t idle_task;   	/* This CPU's idle task */
    bool need_resched;  	/* Flag activated when a user preemption must take place */
    int nr_runnable;    	/* Keeps track of the number of runnable task in this CPU
                            	    -> Note that current is not on the RQ */

    int next_load_balancing;    /* Timestamp of the next simulation step
                                    where load_balancing will take place */
    pthread_mutex_t lock;       /* Runqueue lock*/    
}runqueue_t;


/*Predeclaration for sched_class*/
struct sched_class;

/* Functions exported by the generic scheduler */
slist_t read_task_list_from_file(FILE *file);
void sched_start(slist_t* task_list, struct sched_class* sc);
runqueue_t* get_runqueue_cpu(int cpu);
int write_cpu_log(int cpu, const char *fmt, ...);
void sched_terminate(int errcode);
static inline bool is_idle_task(task_t* t){ return (t->flags & TF_IDLE_TASK); }

/* Scheduling parameters */
extern bool preemptive_scheduler;
extern unsigned int nr_cpus;
extern bool debug_mode;
extern useconds_t tick_delay;
extern int max_simulation_steps;
extern int rr_quantum;
extern int load_balancing_period;

/* This interface must be implemented for each scheduling algorithm */
typedef struct sched_class {
    void (*sched_init)(void);    
    void (*sched_destroy)(void);    
    void (*task_new)(task_t* t);
    task_t* (*pick_next_task)(runqueue_t* rq,int cpu);
    void (*enqueue_task)(task_t* t,int cpu, int runnable);    
    void (*task_tick)(runqueue_t* rq,int cpu);
    task_t* (*steal_task)(runqueue_t* rq,int cpu);    
}sched_class_t;

/* Scheduling class descriptors */
extern sched_class_t rr_mlev_sched;

/* Numerical IDs for the available scheduling algorithms */
enum {
    RR_MLEV_SCHED,
    NR_AVAILABLE_SCHEDULERS
};

typedef struct sched_choice {
        int sched_id;
	char* sched_name;
	sched_class_t* sched_class;
}sched_choice_t;

/* This array contains an entry for each available scheduler */
static const sched_choice_t available_schedulers[NR_AVAILABLE_SCHEDULERS]={
    {RR_MLEV_SCHED,"RR_MLEV",&rr_mlev_sched},
};



#endif
