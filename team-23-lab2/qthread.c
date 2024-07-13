/*
 * file:        qthread.c
 * description: assignment - simple emulation of POSIX threads
 * class:       CS 5600, Fall 2019
 */

/* a bunch of includes which will be useful */

#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <ucontext.h>
#include <unistd.h>

#include "qthread.h"

#define STACK_SIZE 64 * 1024
qthread_t *current;
struct thread_q active;
struct thread_q sleepers;

// push_back function that adds a thread to the FIFO queue
void push_back(struct thread_q *q, qthread_t *thread) {
    if (q->head == NULL && q->tail == NULL)
        q->head = q->tail = thread;
    else {
        q->tail->next_t = thread;
        q->tail = thread;
    }
}

// pop_front function that pops from the head of the FIFO queue
qthread_t *pop_front(struct thread_q *q) {
    qthread_t *front = q->head;
    if (q->head == q->tail)
        q->head = q->tail = NULL;
    else
        q->head = q->head->next_t;

    return front;
}

// empty function to see if the FIFO queue is empty of not
int empty_queue(struct thread_q *q) {
    if (q->head == NULL && q->tail == NULL)
        return 1;
    else
        return 0;
}

/* wrapper function:
 *  - call f(arg)
 *  - if call returns, pass return value to qthread_exit
 */
void qthread_wrapper(void *(*f)(void *), void *arg) {
    /* your code here */
    void *val = f(arg);
    qthread_exit(val);
}

/* qthread_create - see hints for how to implement it, especially:
 * - using getcontext and makecontext
 * - using a "wrapper" function to capture return value of f(arg1)
 *
 * note that the crazy definition of the first argument means:
 *   f is a pointer to a function with args=(void*), returning void*
 */
qthread_t *qthread_create(void *(*f)(void *), void *arg1) {
    /* your code here */
    qthread_t *th = malloc(sizeof(*th)); // Create new thread
    void *stack = malloc(STACK_SIZE);    // Allocate stack for it
    memset(stack, 0xA5, STACK_SIZE);     // Initialize stack with all A5
    th->ctx.uc_stack =
        (stack_t){.ss_sp = stack, .ss_flags = 0, .ss_size = STACK_SIZE};
    getcontext(&th->ctx);
    makecontext(&th->ctx, (f_void_t)qthread_wrapper, 2, f, arg1);
    push_back(&active, th);
    return th;
}

void schedule(void) {
    /* I suggest factoring your code so that you have a 'schedule'
     * function which selects the next thread to run and @switches to it,
     * or goes to sleep if there aren't any threads left to run.
     *
     * NOTE - if you end up switching back to the same thread, do *NOT*
     * use swapcontext - check for this case and return from schedule(),
     * or else you'll crash.
     */

    /*
     * current = [remove head of active]
     * switch to new current
     */
    qthread_t *tmp = current;
again:
    current = pop_front(&active);

    if (current == tmp)
        return;

    if (current == NULL && empty_queue(&sleepers)) {
    	push_back(&active, tmp);
    	goto again;
    }
    else if (current == NULL) {
        usleep(10000);
        while (!empty_queue(&sleepers)) {
            qthread_t *th = pop_front(&sleepers);
            push_back(&active, th);
        }
        goto again;
    }

    if(!current->is_exit)
    	swapcontext(&tmp->ctx, &current->ctx);
    else
    	current = tmp;
}

/* qthread_init - set up a thread structure for the main (OS-provided) thread
 */
void qthread_init(void) {
    /* your code here */
    qthread_t *main_th = malloc(sizeof(*main_th));
    memset(main_th, 0, sizeof(*main_th));
    getcontext(&main_th->ctx);
    current = main_th;
    active.head = active.tail = NULL;
}

/* qthread_yield - yield to the next @runnable thread.
 */
void qthread_yield(void) {
    /* your code here */
    push_back(&active, current);
    schedule();
}

/* qthread_exit, qthread_join - exit argument is returned by
 * qthread_join. Note that join blocks if the thread hasn't exited
 * yet, and is allowed to crash @if the thread doesn't exist.
 */
void qthread_exit(void *val) {
    /* your code here */
    current->is_exit = 1;
    current->return_val = val;

    if (current->wait_t != NULL) {
        qthread_t *tmp = current;
        current = current->wait_t;
        swapcontext(&tmp->ctx, &current->ctx);
    } else {
        schedule();
        // qthread_t *tmp = current;
        // current = pop_front(&active);
        // if (current == NULL)
        //     swapcontext(NULL, &tmp->ctx);
        // else
        //     swapcontext(&tmp->ctx, &current->ctx);
    }
}

void *qthread_join(qthread_t *thread) {
    /* your code here */
    while (!thread->is_exit) {
        thread->wait_t = current;
        qthread_t *tmp = current;
        current = thread;
        swapcontext(&tmp->ctx, &current->ctx);
    }

    void *val = thread->return_val;
    free(thread->stack);
    return val;
}

/* Mutex functions
 */
qthread_mutex_t *qthread_mutex_create(void) {
    qthread_mutex_t *mutex = malloc(sizeof(*mutex));
    memset(mutex, 0, sizeof(*mutex));
    mutex->waiters = malloc(sizeof(*mutex->waiters));
    memset(mutex->waiters, 0, sizeof(*mutex->waiters));
    mutex->waiters->head = mutex->waiters->tail = NULL;
    mutex->locked = 0;

    return mutex;
}

void qthread_mutex_destroy(qthread_mutex_t *mutex) {
    mutex->locked = 0;
    free(mutex->waiters);
    free(mutex);
}

void qthread_mutex_lock(qthread_mutex_t *mutex) {
    if (!mutex->locked)
        mutex->locked = 1;
    else {
        push_back(mutex->waiters, current);
        schedule();
    }
}

void qthread_mutex_unlock(qthread_mutex_t *mutex) {
    if (empty_queue(mutex->waiters))
        mutex->locked = 0;
    else
        push_back(&active, pop_front(mutex->waiters));
}

/* Condition variable functions
 */
qthread_cond_t *qthread_cond_create(void) {
    qthread_cond_t *cond = malloc(sizeof(*cond));
    memset(cond, 0, sizeof(*cond));
    cond->waiters = malloc(sizeof(*cond->waiters));
    memset(cond->waiters, 0, sizeof(*cond->waiters));
    cond->waiters->head = cond->waiters->tail = NULL;

    return cond;
}

void qthread_cond_destroy(qthread_cond_t *cond) {
    free(cond->waiters);
    free(cond);
}

void qthread_cond_wait(qthread_cond_t *cond, qthread_mutex_t *mutex) {
    push_back(cond->waiters, current);
    qthread_mutex_unlock(mutex);
    schedule();
    qthread_mutex_lock(mutex);
}

void qthread_cond_signal(qthread_cond_t *cond) {
    if (!empty_queue(cond->waiters))
        push_back(&active, pop_front(cond->waiters));
}

void qthread_cond_broadcast(qthread_cond_t *cond) {
    while (!empty_queue(cond->waiters))
        push_back(&active, pop_front(cond->waiters));
}

/* Helper function for the POSIX replacement API - you'll need to tell
 * time in order to implement qthread_usleep. WARNING - store the
 * return value in 'unsigned long' (64 bits), not 'unsigned' (32 bits)
 */
unsigned long get_usecs(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000 + tv.tv_usec;
}

/* POSIX replacement API. This semester we're only implementing 'usleep'
 *
 * If there are no runnable threads, your scheduler needs to block
 * waiting for a thread blocked in 'qthread_usleep' to wake up.
 */

/* qthread_usleep - yield to next runnable thread, making arrangements
 * to be put back on the active list after 'usecs' timeout.
 */
void qthread_usleep(long int usecs) {
    unsigned long wakeup = get_usecs() + usecs;
    while (get_usecs() < wakeup) {
        push_back(&sleepers, current);
        schedule();
    }
}
