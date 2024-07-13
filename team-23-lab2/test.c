/*
 * file:        test.c
 * description: tests for Lab 2
 *  Peter Desnoyers, CS 5600, Fall 2023
 *
 * to compile and run you will need to install libcheck:
 *   sudo apt install check
 *
 * libcheck forks a subprocess for each test, so that it can run
 * to completion even if your tests segfault. This makes debugging
 * very difficult; to turn it off do:
 *   CK_FORK=no gdb ./test
 *
 */

#include <check.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "qthread.h"

/* see the libcheck documentation for more details:
 *   https://libcheck.github.io/check/
 * API documentation here:
 *   https://libcheck.github.io/check/doc/doxygen/html/check_8h.html
 */
 
 unsigned long get_usecs(void);
 
START_TEST(test_1) {
    /* your code here */
    ck_assert_int_eq(1, 1);
    ck_assert(1 == 1);
    ck_assert_int_ge(5, 1);
    ck_assert_int_le(2, 3); /* also _gt, _lt, _ne */
    ck_assert_msg(1 == 1, "failed: 1 != 1");
    if (1 != 1)
        ck_abort_msg("impossible result");

    /* if you're testing the sleep function, you might want to
     * use ck_assert_float_eq_tol, which compares two floating point
     * numbers for equality within a specified tolerance.
     *
     *  double t1 = get_usecs();
     *  qthread_usleep(100000);
     *  double t2 = get_usecs();
     *  ck_assert_float_eq_tol(t2-t1, 100000, 100); // += 100 usec
     */
}

START_TEST(thread_queue_push_pop) {
    struct thread_q queue;
    queue.head = NULL;
    queue.tail = NULL;
    qthread_t t1;
    qthread_t t2;
    push_back(&queue, &t1);
    push_back(&queue, &t2);

    ck_assert(&t1 == pop_front(&queue));
    ck_assert(&t2 == pop_front(&queue));
    ck_assert(1 == empty_queue(&queue));
}

START_TEST(thread_queue_push_pop_multiple) {
    struct thread_q queue;
    queue.head = NULL;
    queue.tail = NULL;
    qthread_t t1;
    qthread_t t2;
    qthread_t t3;
    push_back(&queue, &t1);
    push_back(&queue, &t2);
    push_back(&queue, &t3);

    ck_assert(&t1 == pop_front(&queue));
    ck_assert(&t2 == pop_front(&queue));
    ck_assert(&t3 == pop_front(&queue));
    ck_assert(1 == empty_queue(&queue));
}

START_TEST(thread_queue_empty) {
    struct thread_q queue;
    queue.head = NULL;
    queue.tail = NULL;
    qthread_t t1;

    ck_assert(1 == empty_queue(&queue));

    push_back(&queue, &t1);

    ck_assert(0 == empty_queue(&queue));
}

int n;

void *func_basic() {
	n++;
	qthread_yield();
	n += 4;
	qthread_exit(NULL);
}

START_TEST(basic_thread_behavior) {
	n = 0;
	qthread_init();
	qthread_create(func_basic, NULL);
	qthread_yield();
	ck_assert(n == 1);
	qthread_yield();
	qthread_yield();
	qthread_yield();
	qthread_yield();
	ck_assert(n == 5);
	qthread_yield();
	qthread_yield();
	qthread_yield();
}

void *func_int() {
    n = 1;
    qthread_yield();
    n = 2;
    qthread_yield();
    qthread_exit(NULL);
    return NULL;
}

START_TEST(one_thread_context_switching) {
    qthread_init();
    qthread_create(func_int, NULL);
    n = 0;
    qthread_yield();
    ck_assert(n == 1);
    qthread_yield();
    ck_assert(n == 2);
}

void *func_int_2() {
    n++;
    qthread_yield();
    n++;
    qthread_yield();
    qthread_exit(NULL);
    return NULL;
}

void *func_int_3() {
    n += 2;
    qthread_yield();
    n += 2;
    qthread_yield();
    qthread_exit(NULL);
    return NULL;
}

START_TEST(two_threads_context_switching_1) {
    qthread_init();
    qthread_create(func_int_2, NULL);
    qthread_create(func_int_3, NULL);
    n = 0;
    qthread_yield();
    ck_assert(n == 3);
    qthread_yield();
    ck_assert(n == 6);
}

void *func(void *msg) {
    for (int i = 0; i < 10; i++) {
        printf("Thread %s: %d\n", (char *)msg, i);
        usleep(100000);
        qthread_yield();
    }
    qthread_exit(NULL);
    return NULL;
}

START_TEST(two_threads_context_switching_2) {
    qthread_init();
    qthread_create(func, "1");
    qthread_create(func, "2");
    qthread_yield();

    for (int i = 0; i < 9; i++)
        qthread_yield();

    printf("all done!\n");
}

void *join_1() { 
    qthread_exit((int *)123);
    return NULL;
}

START_TEST(join_no_wait) {
    qthread_init();
    qthread_t *t = qthread_create(join_1, NULL);
    qthread_yield();
    void *y = qthread_join(t);
    ck_assert(y == 123);
}

int x;

void *join_2(void *p) { 
    qthread_exit(p);
    return NULL;
}

START_TEST(join_wait) {
    qthread_init();
    qthread_t *t = qthread_create(join_2, &x);
    void *y = qthread_join(t);
    ck_assert(y == &x);
}

qthread_t *target_th;

void *join_th() {
	qthread_join(target_th);
	qthread_exit(NULL);
}

START_TEST(join_same_thread) {
	qthread_init();
	target_th = qthread_create(join_2, &x);
	qthread_t *t1 = qthread_create(join_th, NULL);
	qthread_t *t2 = qthread_create(join_th, NULL);
	qthread_join(t1);
	qthread_join(t2);
}

void *sleep_fn_1(void *msg) {
    qthread_usleep(1000000);
    for (int i = 0; i < 10; i++) {
        printf("Thread %s: %d\n", (char *)msg, i);
    }
    return NULL;
}

void *sleep_fn_2(void *msg) {
    for (int i = 0; i < 10; i++) {
        printf("Thread %s: %d\n", (char *)msg, i);
        qthread_yield();
    }
    return NULL;
}

void *sleep_fn_3(void *msg) {
    for (int i = 0; i < 10; i++) {
        printf("Thread %s: %d\n", (char *)msg, i);
        qthread_yield();
    }
    return NULL;
}

START_TEST(sleep_1) {
    qthread_init();
    qthread_t *t1 = qthread_create(sleep_fn_1, "1");
    qthread_create(sleep_fn_2, "2");
    qthread_create(sleep_fn_3, "3");
    qthread_join(t1);
}

qthread_cond_t *cv1;
qthread_mutex_t *m1;

void *sleep_fn_4(void *msg) {
    qthread_mutex_lock(m1);
    unsigned long time = get_usecs();
    qthread_usleep(10000000);
    unsigned long wakeup = get_usecs() - time;
    printf("Thread %s sleeping since: %lu ms\n", (char *)msg, wakeup);
    qthread_mutex_unlock(m1);
    for (int i = 0; i < 10; i++) {
        qthread_mutex_lock(m1);
    	printf("Thread %s: %d\n", (char *)msg, i);
    	qthread_cond_signal(cv1);
    	qthread_mutex_unlock(m1);
    	qthread_yield();
    }
}

void *sleep_fn_5(void *msg) {
    for (int i = 0; i < 10; i++) {
        qthread_mutex_lock(m1);
    	qthread_cond_wait(cv1, m1);
    	printf("Thread %s: %d\n", (char *)msg, i);
    	qthread_mutex_unlock(m1);
    	qthread_yield();
    }
}

void *sleep_fn_6(void *msg) {
    for (int i = 0; i < 10; i++) {
    	qthread_mutex_lock(m1);
    	printf("Thread %s: %d\n", (char *)msg, i);
    	qthread_mutex_unlock(m1);
    	qthread_yield();
    }
}

START_TEST(sleep_2_timeout) {
    qthread_init();
    cv1 = qthread_cond_create();
    m1 = qthread_mutex_create();
    qthread_t *t1 = qthread_create(sleep_fn_4, "1");
    qthread_create(sleep_fn_5, "2");
    qthread_create(sleep_fn_6, "3");
    qthread_join(t1);
    qthread_cond_destroy(cv1);
    qthread_mutex_destroy(m1);
}

void *sleep_fn_7(void *msg) {
    qthread_mutex_lock(m1);
    qthread_usleep(1000000);
    
    for (int i = 0; i < 10; i++) {
    	printf("Thread %s: %d\n", (char *)msg, i);	
    }
    qthread_mutex_unlock(m1);
    
}

void *sleep_fn_8(void *msg) {
    unsigned long time = get_usecs();
    qthread_mutex_lock(m1);
    unsigned long wakeup = get_usecs() - time;
    if (wakeup < 1100000) 
    	printf("blocking on thread 1 since: %lu ms\n", wakeup);
    
    for (int i = 0; i < 10; i++) {
    	printf("Thread %s: %d\n", (char *)msg, i);
    }
    
    qthread_mutex_unlock(m1);
}

START_TEST(sleep_3) {
    qthread_init();
    m1 = qthread_mutex_create();
    qthread_t *t1 = qthread_create(sleep_fn_7, "1");
    qthread_t *t2 = qthread_create(sleep_fn_8, "2");
    qthread_join(t1);
    qthread_yield();
    qthread_mutex_destroy(m1);
}

void *sleep_fn_9(void *msg) {
    unsigned long time = get_usecs();
    qthread_usleep(1000000);
    unsigned long wakeup = get_usecs() - time;
    printf("Thread %s sleeping since: %lu ms\n", (char *)msg, wakeup);
    qthread_yield();
}

START_TEST(sleep_4) {
    qthread_init();
    qthread_t *t1 = qthread_create(sleep_fn_9, "1");
    qthread_t *t2 = qthread_create(sleep_fn_9, "2");
    qthread_join(t1);
}

START_TEST(sleep_5) {
    qthread_init();
    qthread_t *t1 = qthread_create(sleep_fn_9, "1");
    qthread_t *t2 = qthread_create(sleep_fn_9, "2");
    qthread_t *t3 = qthread_create(sleep_fn_9, "3");
    qthread_join(t1);
}

qthread_mutex_t *m;

void *func_mutex_1_1() {
    qthread_mutex_lock(m);
    n += 1;
    qthread_yield();
    qthread_mutex_unlock(m);
    n += 3;
    qthread_exit(NULL);
    return NULL;
}

void *func_mutex_1_2() {
    qthread_mutex_lock(m);
    n += 7;
    qthread_yield();
    qthread_mutex_unlock(m);
    n += 17;
    qthread_exit(NULL);
    return NULL;
}

START_TEST(mutex_1) {
    qthread_init();
    n = 0;
    m = qthread_mutex_create();
    qthread_t *th1 = qthread_create(func_mutex_1_1, NULL);
    qthread_t *th2 = qthread_create(func_mutex_1_2, NULL);
    qthread_yield();
    ck_assert(1 == n);
    qthread_yield();
    ck_assert(4 == n);
    qthread_yield();
    ck_assert(11 == n);
    qthread_yield();
    ck_assert(28 == n);
    qthread_join(th1);
    qthread_join(th2);
    qthread_mutex_destroy(m);
}

int t;

void *func_mutex_2() {
    n += 1;
    qthread_mutex_lock(m);
    n += 2;
    qthread_yield();
    qthread_yield();
    qthread_mutex_unlock(m);
    //n += 4;
    qthread_exit(NULL);
    return NULL;
}

START_TEST(mutex_2) {
    n = 0;
    qthread_init();
    m = qthread_mutex_create();
    qthread_mutex_lock(m);
    qthread_t *th = qthread_create(func_mutex_2, NULL);
    qthread_yield();
    qthread_yield();
    ck_assert(n == 1);
    qthread_mutex_unlock(m);
    qthread_join(th);
    ck_assert(n == 3);
    qthread_mutex_destroy(m);
}

START_TEST(mutex_one_thread) {
	n = 0;
	qthread_init();
	m = qthread_mutex_create();
	qthread_mutex_lock(m);
	n++;
	ck_assert(n == 1);
	qthread_mutex_unlock(m);
	n += 2;
	ck_assert(n == 3);
	qthread_mutex_destroy(m);
}

void mutex_two() {
	qthread_mutex_lock(m);
	n += 3;
	qthread_yield();
	qthread_yield();
	qthread_mutex_unlock(m);
	qthread_exit(NULL);
}

START_TEST(mutex_two_threads) {
	n = 0;
	qthread_init();
	m = qthread_mutex_create();
	qthread_t *t = qthread_create(mutex_two, NULL);
	qthread_mutex_lock(m);
	n++;
	ck_assert(n == 1);
	qthread_yield();
	qthread_yield();
	qthread_yield();
	ck_assert(n == 1);
	qthread_mutex_unlock(m);
	qthread_yield();
	qthread_yield();
	ck_assert(n == 4);
	qthread_mutex_destroy(m);
}

void mutex_three_1() {
	n += 3;
	qthread_mutex_lock(m);
	n += 5;
	qthread_yield();
	qthread_yield();
	qthread_mutex_unlock(m);
	qthread_exit(NULL);
}

void mutex_three_2() {
	n += 7;
	qthread_mutex_lock(m);
	n += 11;
	qthread_yield();
	qthread_yield();
	qthread_mutex_unlock(m);
	qthread_exit(NULL);
}

START_TEST(mutex_three_threads) {
	n = 0;
	qthread_init();
	m = qthread_mutex_create();
	qthread_t *t1 = qthread_create(mutex_three_1, NULL);
	qthread_t *t2 = qthread_create(mutex_three_2, NULL);
	qthread_mutex_lock(m);
	qthread_yield();
	qthread_yield();
	qthread_yield();
	ck_assert(n == 10);
	qthread_mutex_unlock(m);
	qthread_yield();
	qthread_yield();
	qthread_yield();
	ck_assert(n == 15);
	qthread_yield();
	qthread_yield();
	qthread_yield();
	ck_assert(n == 26);
	qthread_mutex_destroy(m);
}

qthread_cond_t *cv;

void *func_cond_1() {
	qthread_mutex_lock(m);
	n += 3;
	qthread_cond_signal(cv);
	qthread_mutex_unlock(m);
	qthread_yield();
	qthread_yield();
	qthread_yield();
	qthread_exit(NULL);
}

START_TEST(cond_1) {
	n = 0;
	qthread_init();
	m = qthread_mutex_create();
	cv = qthread_cond_create();
	qthread_t *t = qthread_create(func_cond_1, NULL);
	ck_assert(n == 0);
	qthread_mutex_lock(m);
	ck_assert(n == 0);
	qthread_cond_wait(cv, m);
	ck_assert(n == 3);
	qthread_mutex_unlock(m);
	qthread_join(t);
	ck_assert(n == 3);
	
	qthread_mutex_unlock(m);
	qthread_mutex_destroy(m);
	qthread_cond_destroy(cv);
}

void *func_cond_2_1() {
	qthread_mutex_lock(m);
	qthread_cond_wait(cv, m);
	n += 3;
	qthread_mutex_unlock(m);
	qthread_exit(NULL);
}

void *func_cond_2_2() {
	qthread_mutex_lock(m);
	qthread_cond_wait(cv, m);
	n += 5;
	qthread_mutex_unlock(m);
	qthread_exit(NULL);
}

START_TEST(cond_2) {
	n = 0;
	qthread_init();
	m = qthread_mutex_create();
	cv = qthread_cond_create();
	qthread_t *t1 = qthread_create(func_cond_2_1, NULL);
	qthread_t *t2 = qthread_create(func_cond_2_2, NULL);
	ck_assert(n == 0);
	qthread_yield();
	qthread_mutex_lock(m);
	ck_assert(n == 0);
	qthread_cond_signal(cv);
	qthread_mutex_unlock(m);
	qthread_yield();
	qthread_yield();
	qthread_yield();
	qthread_mutex_lock(m);
	ck_assert(n == 3);
	qthread_cond_signal(cv);
	qthread_mutex_unlock(m);
	qthread_yield();
	qthread_yield();
	qthread_yield();
	ck_assert(n == 8);
	
	qthread_join(t1);
	qthread_join(t2);
	
	qthread_mutex_unlock(m);
	qthread_mutex_destroy(m);
	qthread_cond_destroy(cv);
}

START_TEST(cond_broadcast) {
	n = 0;
	qthread_init();
	m = qthread_mutex_create();
	cv = qthread_cond_create();
	qthread_t *t1 = qthread_create(func_cond_2_1, NULL);
	qthread_t *t2 = qthread_create(func_cond_2_2, NULL);
	ck_assert(n == 0);
	qthread_yield();
	qthread_mutex_lock(m);
	ck_assert(n == 0);
	qthread_cond_broadcast(cv);
	qthread_mutex_unlock(m);
	qthread_yield();
	qthread_yield();
	qthread_yield();
	qthread_yield();
	qthread_yield();
	ck_assert(n == 8);
	qthread_join(t1);
	qthread_join(t2);
	
	qthread_mutex_unlock(m);
	qthread_mutex_destroy(m);
	qthread_cond_destroy(cv);

}

Suite *create_suite(void) {
    Suite *s = suite_create("qthreads");
    TCase *tc = NULL;

    /* you can add all your tests to a single "test case"
     */
    tc = tcase_create("example test set");
    tcase_add_test(tc, test_1);
    suite_add_tcase(s, tc);

    /* or you can add some structure by adding more test cases:
     *  tc = tcase_create("another test set");
     *  tcase_add_test(tc, test_x);
     *   ...
     *  suite_add_tcase(s, tc);
     */

    tc = tcase_create("queue test set");
    tcase_add_test(tc, thread_queue_push_pop);
    tcase_add_test(tc, thread_queue_push_pop_multiple);
    tcase_add_test(tc, thread_queue_empty);

    suite_add_tcase(s, tc);

    tc = tcase_create("basic qthread context switching test set");
    tcase_add_test(tc, one_thread_context_switching);
    tcase_add_test(tc, two_threads_context_switching_1);
    tcase_add_test(tc, two_threads_context_switching_2);
    tcase_add_test(tc, basic_thread_behavior);
    suite_add_tcase(s, tc);

    tc = tcase_create("join test set");
    tcase_add_test(tc, join_no_wait);
    tcase_add_test(tc, join_wait);
    tcase_add_test(tc, join_same_thread);
    suite_add_tcase(s, tc);

    tc = tcase_create("sleep test set");
    tcase_add_test(tc, sleep_1);
    tcase_add_test(tc, sleep_2_timeout);
    tcase_add_test(tc, sleep_3);
    tcase_add_test(tc, sleep_4);
    tcase_add_test(tc, sleep_5);
    suite_add_tcase(s, tc);

    tc = tcase_create("mutex test set");
    tcase_add_test(tc, mutex_1);
    tcase_add_test(tc, mutex_2);
    tcase_add_test(tc, mutex_one_thread);
    tcase_add_test(tc, mutex_two_threads);
    tcase_add_test(tc, mutex_three_threads);
    suite_add_tcase(s, tc);

    tc = tcase_create("cond test set");
    tcase_add_test(tc, cond_1);
    tcase_add_test(tc, cond_2);
    tcase_add_test(tc, cond_broadcast);
    suite_add_tcase(s, tc);

    return s;
}

int main(int argc, char **argv) {
    Suite *s = create_suite();
    SRunner *sr = srunner_create(s);

    srunner_run_all(sr, CK_VERBOSE);
    int n_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    printf("%d tests failed\n", n_failed);
}
