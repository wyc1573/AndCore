//
// Created by wyc on 2023/8/16.
//
#include "signal_test.h"
#include <signal.h>
#include <unistd.h>
#include <setjmp.h>
#include "android-base/logging.h"

/**longjmp & signal & alarm*/
// void (*signal(int signo, void(*func)(int)))(int);
// void (int) (*) (int signo, void(*func)(int));
static void sig_alarm(int signo) {
    LOG(INFO) << "sig_alarm handler signo = " << signo;
}

unsigned int sleep1(unsigned int seconds) {
    if (signal(SIGALRM, sig_alarm) == SIG_ERR) {
        return seconds;
    }
    alarm(seconds);
    pause();
    return (alarm(0));
}

static jmp_buf env_alarm;

static void sig_alarm2(int signo) {
    longjmp(env_alarm, 1);
}

// sleep2仍然是有问题的
unsigned int sleep2(unsigned int seconds) {
    if (signal(SIGALRM, sig_alarm2) == SIG_ERR) {
        return seconds;
    }
    if(setjmp(env_alarm) == 0) {
        alarm(seconds);
        pause();
    }
    return alarm(0);
}

static jmp_buf env_another;
void another_alarm() {
    int n;
    char line[1024 * 4];
    if (signal(SIGALRM, sig_alarm) == SIG_ERR) {
        return;
    }
    alarm(10); //设置超时上限
    // 如果系统调用是自动重启的 超时无效
    if ((n = read(STDIN_FILENO, line, 1024 * 4)) < 0) {
        LOG(INFO) << "read error!";
    }
    alarm(0);
}

static jmp_buf env_another2;
static void sig_alarm_another(int signo) {

}

void another_alarm2() {
    int n;
    char line[1024*4];
    if (signal(SIGALRM, sig_alarm) == SIG_ERR) {
        return;
    }
    if (setjmp(env_another2) != 0) {
        return;
    }
    alarm(10);
    if ((n = read(STDIN_FILENO, line, 1024*4)) < 0) {
        return;
    }
    alarm(0);
    //...
}

void test_alarm() {
    int r = alarm(10);
    LOG(INFO) << "alarm(10) return " << r;
    sleep(2);
    r = alarm(0); //取消闹钟，返回之前剩余的时间
    LOG(INFO) << "alarm(0) return " << r;
}

static jmp_buf env_test_jmp;
static void sig_alarm3(int signo) {
    longjmp(env_test_jmp, 1);
    LOG(INFO) << "sig_alarm3 done.";
}

void test_longjmp() {
    LOG(INFO) << "Enter test_longjmp";
    if(signal(SIGALRM, sig_alarm3) == SIG_ERR) {
        return;
    }
    alarm(3);
    if (setjmp(env_test_jmp) == 0) {
        LOG(INFO) << "test longjmp, first enter, sleep 10s";
        sleep(5);
        LOG(INFO) << "sleep done!";
    }
    LOG(INFO) << "Exit test_longjmp";
}

/*****************sigset**********************/
void test_sigset() {
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGALRM);
    sigismember(&set, SIGALRM);
    sigdelset(&set, SIGALRM);
    sigfillset(&set);
}

void pr_mask(const char* str) {
    sigset_t oset;
    sigset_t set;

    int errno_save;
    errno_save = errno;

    sigaddset(&set, SIGQUIT);
    sigprocmask(SIG_UNBLOCK, &set, &oset);
    LOG(INFO) << str;
    // 用于单线程进程
/*    if (sigprocmask(0, NULL, &oset) < 0) {
        LOG(INFO) << "sigprocmask error";
        return;
    } else {*/
        kill(getpid(), SIGQUIT);
        sigpending(&oset);
        if (sigismember(&oset, SIGQUIT)) {
            LOG(INFO) << "SIGQUIT";
        }
        if (sigismember(&oset, SIGINT)) {
            LOG(INFO) << "SIGINT";
        }
        if (sigismember(&oset, SIGSEGV)) {
            LOG(INFO) << "SIGSEGV";
        }
        if (sigismember(&oset, SIGUSR1)) {
            LOG(INFO) << "SIGUSR1";
        }
        if (sigismember(&oset, SIGALRM)) {
            LOG(INFO) << "SIGALRM";
        }
        LOG(INFO) << ".";
//    }
    errno = errno_save;
}

typedef void(Sigfunc)(int) ;
//用sigaction模拟signal
Sigfunc* signal1(int signo, Sigfunc* func) {
    struct sigaction act, oact;
    act.sa_handler = func;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    if (signo == SIGALRM) {
#ifdef SA_INTERRUPT
        act.sa_flags |= SA_INTERRUPT;
#endif
    } else {
        act.sa_flags |= SA_RESTART;
    }

    if (sigaction(signo, &act, &oact) < 0) {
        return SIG_ERR;
    }
    return oact.sa_handler;
}

Sigfunc* signal2(int signo, Sigfunc* func) {
    struct sigaction act, oact;
    act.sa_handler = func;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
#ifdef SA_INTERRUPT
    act.sa_flags |= SA_INTERRUPT;
#endif
    if (sigaction(signo, &act, &oact) < 0) {
        return SIG_ERR;
    }
    return oact.sa_handler;
}

/**************sigsetjmp & siglongjmp****************/
static void sig_usr1(int);
static void sig_alarm_jmp(int);
static sigjmp_buf sig_jmpbuf;
static volatile sig_atomic_t canjump;

void test_sigjmp() {
    if (signal(SIGUSR1, sig_usr1) == SIG_ERR) {
        return;
    }
    if (signal(SIGALRM, sig_alarm_jmp) == SIG_ERR) {
        return;
    }
    pr_mask("starting main: ");
    if (sigsetjmp(sig_jmpbuf, 1)) {
        pr_mask("ending main: ");
        exit(0);
    }

    canjump = 1;
    for(; ;) pause();
}

static void sig_usr1(int signo) {
    time_t starttime;
    if (canjump == 0) return;
    pr_mask("starting sig_usr1: ");

    alarm(3);
    starttime = time(NULL);

    for(;;) {
        if(time(NULL) > starttime + 5)
            break;
    }
    canjump = 0;
    siglongjmp(sig_jmpbuf, 1);
}

static void sig_alarm_jmp(int signo) {
    pr_mask("in sig_alarm: ");
}

/****************sigsuspend*******************/
static void sig_int(int signo);
void test_sigsuspend() {
    sigset_t newmask, oldmask, waitmask;

    pr_mask("program start:");
    if (signal(SIGINT, sig_int) == SIG_ERR) {
        return;
    }
    sigemptyset(&waitmask);
    sigaddset(&waitmask, SIGUSR1);
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGINT);

    if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0) {
        return;
    }
    pr_mask("in critical region: ");
    // 先恢复屏蔽字 然后等待到来
    // sigsuspend推出前 恢复屏蔽字为函数调用之前的值
    if (sigsuspend(&waitmask) != -1) {
        return;
    }
    pr_mask("after return from sigsuspend");
    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0) {
        return;
    }

    pr_mask("program exit:");
    exit(0);
}

static void sig_int(int signo) {
    pr_mask("in sig_int:");
}

volatile sig_atomic_t quitflag;
static void sig_int_quit(int signo) {
    if (signo == SIGINT) {
        LOG(INFO) << "interrupt";
    } else if (signo == SIGQUIT) {
        quitflag = 1;
    }
}
// 等待信号发生
void test_sigsuspend2() {
    sigset_t newmask, oldmask, zeromask;
    if (signal(SIGINT, sig_int_quit) == SIG_ERR) {
        return;
    }
    if (signal(SIGQUIT, sig_int_quit) == SIG_ERR) {
        return;
    }
    sigemptyset(&zeromask);
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGQUIT);

    if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0) {
        return;
    }

    while (quitflag == 0) {
        sigsuspend(&zeromask);
    }
    quitflag = 0;
    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0) {
        return;
    }
}

//父子进程同步
/**
 * SIGUSR1: 由父进程发送给子进程
 * SIGUSR2: 由子进程发送给父进程
 * */
static volatile sig_atomic_t sigflag;
static sigset_t newmask, oldmask, zeromask;

static void sig_usr2(int signo) {
    sigflag = 1;
}

void TELL_WAIT(void) {
    if (signal(SIGUSR1, sig_usr2) == SIG_ERR) {
        return;
    }
    if (signal(SIGUSR2, sig_usr2) == SIG_ERR) {
        return;
    }
    sigemptyset(&zeromask);
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGUSR1);
    sigaddset(&newmask, SIGUSR2);

    if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0) {
        return;
    }
}

// 子进程发给父进程
void TELL_PARENT(pid_t pid) {
    kill(pid, SIGUSR2);
}

void WAIT_PARENT(void) {
    while (sigflag == 0) sigsuspend(&zeromask);
    sigflag = 0;
    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0) {
        return;
    }
}

// 父进程发给子进程
void TELL_CHILD(pid_t pid) {
    kill(pid, SIGUSR1);
}

void WAIT_CHILD(void) {
    while (sigflag == 0) sigsuspend(&zeromask);
    sigflag = 0;
    if (sigprocmask(SIG_SETMASK, &oldmask, NULL)< 0) {
        return;
    }
}

/***************abort*****************/
//实现abort
void abort() {
    sigset_t mask;
    struct sigaction action;

    sigaction(SIGABRT, NULL, &action);
    if (action.sa_handler == SIG_IGN) {
        action.sa_handler = SIG_DFL;
        sigaction(SIGABRT, &action, NULL);
    }

    if (action.sa_handler == SIG_DFL) {
        fflush(NULL);
    }

    // caller can't block
    sigfillset(&mask);
    sigdelset(&mask, SIGABRT);
    sigprocmask(SIG_SETMASK, &mask, NULL);
    // kill返回的时候 目标进程如果没有阻塞该信号 就已经接收到该信号
    // 测试发现同一个线程执行的话 kill返回时已经执行完信号处理函数
    // 多线程环境则没有执行信号处理函数就返回
    kill(getpid(), SIGABRT);

    // process caught SIGABRT and returned
    fflush(NULL);
    action.sa_handler = SIG_DFL;
    sigaction(SIGABRT, &action, NULL);
    sigprocmask(SIG_SETMASK, &mask, NULL);
    kill(getpid(), SIGABRT);
    exit(1);
}

/***************sleep nanosleep clock_nanosleep*******************/
void test_sleep() {
    sleep(1);
    struct timespec req {.tv_sec = 5, .tv_nsec = 0};
    struct timespec rem;
    nanosleep(&req, &rem);
    clock_nanosleep(CLOCK_THREAD_CPUTIME_ID, 0, &req, &rem);
    clock_nanosleep(CLOCK_REALTIME, 0, &req, &rem);

    clock_gettime(CLOCK_REALTIME, &req);
    LOG(INFO) << "CLOCK_REALTIME: tv_sec = " << req.tv_sec << ", tv_nsec = " << req.tv_nsec;

    clock_gettime(CLOCK_MONOTONIC, &req);
    LOG(INFO) << "CLOCK_MONOTONIC: tv_sec = " << req.tv_sec << ", tv_nsec = " << req.tv_nsec;

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &req);
    LOG(INFO) << "CLOCK_PROCESS_CPUTIME_ID: tv_sec = " << req.tv_sec << ", tv_nsec = " << req.tv_nsec;

    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &req);
    LOG(INFO) << "CLOCK_THREAD_CPUTIME_ID: tv_sec = " << req.tv_sec << ", tv_nsec = " << req.tv_nsec;
}

/*******************sigqueue******************/
// 信号排队 + 多传递sigval信息
// int sigqueue(pid_t pid, int signo, const union sigval value);

/****************psignal psiginfo strsignal*************/
void pr_signal() {
    LOG(INFO) << strsignal(SIGQUIT);
}

/***************线程和信号****************/
#include <pthread.h>
int quitflag2 = 0;
sigset_t mask;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t waitlock = PTHREAD_COND_INITIALIZER;

void* thr_fn(void* arg) {
    int err, signo;
    for (;;) {
        err = sigwait(&mask, &signo);
        if (err != 0) {
            exit(-1);
        }
        switch(signo) {
            case SIGINT:
                LOG(INFO) << "interrupt";
                break;
            case SIGQUIT:
                pthread_mutex_lock(&lock);
                quitflag2 = 1;
                pthread_mutex_unlock(&lock);
                pthread_cond_signal(&waitlock);
                return (0);
            default:
                LOG(INFO) << "unexpected signal";
                exit(1);
        }
    }
}

void test_thread_signal() {
    int err;
    sigset_t oldmask;
    pthread_t tid;

    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGQUIT);

    if ((err = pthread_sigmask(SIG_BLOCK, &mask, &oldmask)) != 0) {
        exit(-1);
    }

    err = pthread_create(&tid, NULL, thr_fn, 0);
    if (err != 0) {
        exit(-1);
    }

    pthread_mutex_lock(&lock);
    while (quitflag2 == 0) {
        pthread_cond_wait(&waitlock, &lock);
    }
    pthread_mutex_unlock(&lock);

    quitflag2 = 0;

    if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0) {
        LOG(INFO) << "SIG_SETMASK error";
    }
    return;
}

void sa_sigaction(int, siginfo_t*, void*) {
    LOG(INFO) << "sa_sigaction Tid: " << gettid();
}

void* thr_fn2(void* arg) {
    int signo;
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);

    LOG(INFO) << "in Tid: " << gettid();
//    sigwait(&set, &signo);
//    LOG(INFO) << "sig wait return";
    for(;;) {
        pause();
    }
}

void test_pthread_kill() {
    struct sigaction act, oact;
    pthread_t tid;

    sigset_t mask, omask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    if (sigprocmask(SIG_UNBLOCK, &mask, &omask) < 0) {
        return;
    }

    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    act.sa_flags |= SA_SIGINFO;
    act.sa_sigaction = sa_sigaction;

    if (sigaction(SIGUSR1, &act, &oact) < 0) {
        return;
    }

    pthread_create(&tid, NULL, thr_fn2, 0);

    sleep(1);
    pthread_kill(tid, SIGUSR1);

    if (sigprocmask(SIG_SETMASK, &omask, NULL) < 0) {
        LOG(INFO) << "SIG_SETMASK error";
    }
}


#include <thread>
void signal_test() {
    LOG(INFO) << __FUNCTION__ ;
    test_pthread_kill();
}
