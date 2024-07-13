/*
 * file:        qthread.h
 * description: assignment - simple emulation of POSIX threads
 * class:       CS 5600, Fall 2019
 */
#ifndef __QTHREAD_H__
#define __QTHREAD_H__

#include <ucontext.h> /* you'll need this */

/* this is your qthread structure.
 */
typedef struct qthread {
    ucontext_t ctx;
    /* ... your code here */
    struct qthread *next_t; // next thread pointer
    int is_exit;            // boolean flag indicating 1: true 0: false
                            // whether qthread_exit has been called
    void *return_val;       // return value for qthread_join
    struct qthread *wait_t; // pointer to thread waiting in qthread_join
    void *stack; // pointer to the stack allocated that could free it in
                 // qthread_join
} qthread_t;

/* You'll need some sort of way of keeping lists and queues of threads.
 * suggestion:
 *  - add a 'next' pointer to the thread structure
 *  - create a queue structure, and 'push_back' and 'pop_front'
 *    functions that add and remove thread structures from the queue
 */
struct thread_q {
    /* your code here */
    qthread_t *head; // head pointer
    qthread_t *tail; // tail pointer
};

/* Mutex and cond structures - @allocate them in qthread_mutex_create /
 * qthread_cond_create and free them in @the corresponding _destroy functions.
 */
typedef struct qthread_mutex {
    /* your code here */
    int locked;
    struct thread_q *waiters;
} qthread_mutex_t;

typedef struct qthread_cond {
    /* your code here */
    struct thread_q *waiters;
} qthread_cond_t;

/* You'll need to cast the function argument to makecontext
 * to f_void_t
 */
typedef void (*f_void_t)(void);

/* thread queue operations
 */
void push_back(struct thread_q *q, qthread_t *thread);
qthread_t *pop_front(struct thread_q *q);
int empty_queue(struct thread_q *q);

/* prototypes - see qthread.c for function descriptions
 */

void qthread_init(void);
qthread_t *qthread_create(void *(*f)(void *), void *arg1);
void qthread_yield(void);
void qthread_exit(void *val);
void *qthread_join(qthread_t *thread);
qthread_mutex_t *qthread_mutex_create(void);
void qthread_mutex_lock(qthread_mutex_t *mutex);
void qthread_mutex_unlock(qthread_mutex_t *mutex);
void qthread_mutex_destroy(qthread_mutex_t *mutex);
qthread_cond_t *qthread_cond_create(void);
void qthread_cond_wait(qthread_cond_t *cond, qthread_mutex_t *mutex);
void qthread_cond_signal(qthread_cond_t *cond);
void qthread_cond_broadcast(qthread_cond_t *cond);
void qthread_cond_destroy(qthread_cond_t *cond);
void qthread_usleep(long int usecs);

#endif
