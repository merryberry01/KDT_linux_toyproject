#include <stdio.h>
#include <sys/prctl.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>

#include <system_server.h>
#include <gui.h>
#include <input.h>
#include <web_server.h>

static int toy_timer = 0;

void timer_handler(int sig, siginfo_t *info, void *ucontext){
    printf("toy_timer: %d\n", ++toy_timer);
}

int system_server()
{
    struct itimerspec ts;
    struct sigaction  sa;
    struct sigevent   sev;
    timer_t *tidlist;

    printf("나 system_server 프로세스!\n");

    /* 5초 타이머를 만들어 봅시다. */
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = timer_handler; //for SA_SIGINFO flag
    sigemptyset(&sa.sa_mask);
    if(sigaction(SIGALRM, &sa, NULL) == -1)
        perror("sigaction() in system_server");

    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGALRM;
    if(!(tidlist = (timer_t *)malloc(sizeof(timer_t))))
        perror("malloc() in system_server");
    if(timer_create(CLOCK_REALTIME, &sev, tidlist) == -1)
        perror("timer_create() in system_server");

    ts.it_value.tv_sec = 5;
    ts.it_value.tv_nsec = 0;
    ts.it_interval.tv_sec = 5;
    ts.it_interval.tv_nsec = 0;
    if(timer_settime(*tidlist, 0, &ts, NULL) == -1)
        perror("timer_settime() in system_server");

    while(1)
        sleep(1);
    free(tidlist);
    return 0;
}

int create_system_server()
{
    pid_t systemPid;
    const char *name = "system_server";

    printf("여기서 시스템 프로세스를 생성합니다.\n");

    /* fork 를 이용하세요 */
    switch (systemPid = fork()) {
    case -1:
        printf("fork failed\n");
    case 0:
        /* 프로세스 이름 변경 */
        if (prctl(PR_SET_NAME, (unsigned long) name) < 0)
            perror("prctl()");
        system_server();
        break;
    default:
        break;
    }

    return systemPid;
}
