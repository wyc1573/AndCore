//
// Created by wyc on 2023/3/16.
//

#pragma once
#include <pthread.h>
#include <stdatomic.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include "base/futextest.h"
#include "base/logging.h"
#include "android-base/stringprintf.h"

static futex_t f1 = FUTEX_INITIALIZER;
static futex_t f2 = FUTEX_INITIALIZER;

static int waiter_tid;
static int create_rt_thread(pthread_t *pth, void*(*func)(void *), void *arg, int policy)
{
    struct sched_param schedp;
    pthread_attr_t attr;
    int ret;

    pthread_attr_init(&attr);
    memset(&schedp, 0, sizeof(schedp));

    ret = pthread_attr_setschedpolicy(&attr, policy);

    if (ret) {
        LOG(INFO) << "pthread_attr_setschedpolicy " << ret;
        return -1;
    }

    ret = pthread_create(pth, &attr, func, (void*) pth);

    if (ret) {
        PLOG(FATAL) << "pthread_create " << ret;
    }
    return 0;
}


static void *waiterfn(void *arg)
{
    unsigned int old_val;
    int ret = 0;
    waiter_tid = pthread_gettid_np(*(pthread_t*)arg);
    LOG(INFO) << "[waiterfn]: Waiter running, pthread = " << waiter_tid;
    PLOG(INFO) << "[waiterfn]: before futex_wait_requeue_pi, f1 = " << f1 << ", f2 = " << f2;
    old_val = f1;
    ret = futex_wait_requeue_pi(&f1, old_val, &f2, NULL,
                                FUTEX_PRIVATE_FLAG);
    PLOG(INFO) << "[waiterfn]: after futex_wait_requeue_pi ret = " << ret;
//    int    res = futex_unlock_pi(&f2, FUTEX_PRIVATE_FLAG);
//    PLOG(INFO) << "[test_futex_requeue_pi]: futex_unlock_pi return " << res;
    pthread_exit(NULL);
}

static void test_futex_requeue_pi() {
    pthread_t waiter;
    pthread_t waiter2;
    int old_val = f1, res = 0;
    res = create_rt_thread(&waiter, waiterfn, NULL, SCHED_NORMAL);
    if (res) {
        PLOG(FATAL) << "Creating waiting thread failed" << res;
    }
    res = create_rt_thread(&waiter2, waiterfn, NULL, SCHED_NORMAL);
    if (res) {
        PLOG(FATAL) << "Creating waiting thread failed" << res;
    }
    sleep(2);
    LOG(INFO) << "[test_futex_requeue_pi]: before futex_cmp_requeue_pi, f1 = " << f1 << ", f2 = " << f2;
    // 只有一个线程的时候， requeue的时候拿到了锁
    res = futex_cmp_requeue_pi(&f1, old_val, &(f2), 1, 1,
                               FUTEX_PRIVATE_FLAG);
    LOG(INFO) << "[test_futex_requeue_pi]: after futex_cmp_requeue_pi, f1 = " << f1 << ", f2 = " << f2;
    LOG(INFO) << "[test_futex_requeue_pi]: futex_cmp_requeue_pi return " << res;
    LOG(INFO) <<  "[test_futex_requeue_pi]: Calling FUTEX_UNLOCK_PI on mutex = " << f2;
    res = futex_unlock_pi(&f2, FUTEX_PRIVATE_FLAG);
    PLOG(INFO) << "[test_futex_requeue_pi]: futex_unlock_pi return " << res;
    pthread_join(waiter, NULL);
}


/*---------------------------------------------------------------------*/
static uint32_t *futex1, *futex2, *iaddr;

static int futex_2(uint32_t *uaddr, int futex_op, uint32_t val, const struct timespec *timeout, uint32_t *uaddr2, uint32_t val3){
    return syscall(SYS_futex, uaddr, futex_op, val,timeout, uaddr2, val3);
}

/* Acquire the futex pointed to by 'futexp': wait for its value to
   become 1, and then set the value to 0. */

static void fwait(uint32_t *futexp)
{
    long s;
    while (1) {

        /* Is the futex available? */
        const uint32_t one = 1;
        if (futex_cmpxchg(futexp, one, 0))
            break;      /* Yes */

        /* Futex is not available; wait. */

        s = futex_2(futexp, FUTEX_WAIT, 0, NULL, NULL, 0);
        if (s == -1 && errno != EAGAIN)
            PLOG(INFO) << "[fwait]- " << getpid() << "futex-FUTEX_WAIT";
        else LOG(INFO) << "[fwait]- " << getpid() << "futex-FUTEX_WAIT";
    }
}


/* Release the futex pointed to by 'futexp': if the futex currently
   has the value 0, set its value to 1 and the wake any futex waiters,
   so that if the peer is blocked in fwait(), it can proceed. */

static void fpost(uint32_t *futexp)
{
    long s;

    /* atomic_compare_exchange_strong() was described
       in comments above. */

    const uint32_t zero = 0;
    if (!futex_cmpxchg(futexp, zero, 1)) {
        s = futex_2(futexp, FUTEX_WAKE, 1, NULL, NULL, 0);
        if (s  == -1)
            PLOG(INFO) <<  "[fpost]- " << getpid() << "futex-FUTEX_WAKE";
        else LOG(INFO) << "[fpost]- " << getpid() << " futex-FUTEX_WAKE";
    }
}

void test_futex_normal() {
    pid_t childPid;
    int nloops = 2;

    iaddr = (uint32_t *) mmap(NULL, sizeof(uint32_t) * 2, PROT_READ | PROT_WRITE,
                              MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (iaddr == MAP_FAILED) {
        PLOG(INFO) << "[test_futex_2]: mmap failed!";
    }
    futex1 = &iaddr[0];
    futex2 = &iaddr[1];

    *futex1 = 0;        /* State: unavailable */
    *futex2 = 1;        /* State: available */

    /* Create a child process that inherits the shared anonymous
        mapping. */
    childPid = fork();
    if (childPid == -1)
        PLOG(FATAL) << "[test_futex_2]: fork";

    if (childPid == 0) {        /* Child */
        for (int j = 0; j < nloops; j++) {
            fwait(futex1);
            LOG(INFO) << "Child  " << (intmax_t) getpid();
            fpost(futex2);
        }
        exit(EXIT_SUCCESS);
    }

    sleep(1);

    /* Parent falls through to here. */
    for (int j = 0; j < nloops; j++) {
        fwait(futex2);
        LOG(INFO) << "Parent  " << (intmax_t) getpid();
        fpost(futex1);
    }
    wait(NULL);
}
/*---------------------------------------------------------------------*/


void test_futex() {
    test_futex_normal();
    test_futex_requeue_pi();
}

