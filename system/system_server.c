#include <assert.h>
#include <pthread.h>
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

void *watchdog_thread(void *arg){
    while(1) sleep(1);
}

void *monitor_thread(void *arg){
    while(1) sleep(1);
}

void *disk_service_thread(void *arg){
    while(1) sleep(1);
}

void *camera_service_thread(void *arg){
    while(1) sleep(1);
}

static void timer_expire_signal_handler()
{
    toy_timer++;
}

void set_periodic_timer(long sec_delay, long usec_delay)
{
	struct itimerval itimer_val = {
		 .it_interval = { .tv_sec = sec_delay, .tv_usec = usec_delay },
		 .it_value = { .tv_sec = sec_delay, .tv_usec = usec_delay }
    };

	setitimer(ITIMER_REAL, &itimer_val, (struct itimerval*)0);
}

int posix_sleep_ms(unsigned int timeout_ms)
{
    struct timespec sleep_time;

    sleep_time.tv_sec = timeout_ms / MILLISEC_PER_SECOND;
    sleep_time.tv_nsec = (timeout_ms % MILLISEC_PER_SECOND) * (NANOSEC_PER_USEC * USEC_PER_MILLISEC);

    return nanosleep(&sleep_time, NULL);
}

int system_server()
{
    struct itimerspec ts;
    struct sigaction  sa;
    struct sigevent   sev;
    timer_t *tidlist;
    int retcode;
    pthread_t watchdog_thread_tid, monitor_thread_tid, disk_service_thread_tid, camera_service_thread_tid;

    printf("나 system_server 프로세스!\n");

    /* 5초 타이머를 만들어 봅시다. */
    signal(SIGALRM, timer_expire_signal_handler);
    /* 5초 타이머 등록 */
    set_periodic_timer(5, 0);

    /* watchdog, monitor, disk_service, camera_service 스레드를 생성한다. */
    if(pthread_create(&watchdog_thread_tid, NULL, watchdog_thread, NULL) == -1)
        perror("pthread_create(watchdog_thread) in system_server()");
    if(pthread_create(&monitor_thread_tid, NULL, monitor_thread, NULL) == -1)
        perror("pthread_create(monitor_thread) in system_server()");
    if(pthread_create(&disk_service_thread_tid, NULL, disk_service_thread, NULL) == -1)
        perror("pthread_create(disk_service_thread) in system_server()");
    if(pthread_create(&camera_service_thread_tid, NULL, camera_service_thread, NULL) == -1)
        perror("pthread_create(camera_service_thread) in system_server()");

    printf("system init done.  waiting...");
    while (1) {
        sleep(1);
    }

    if(pthread_join(watchdog_thread_tid, NULL) == -1)
        perror("pthread_join(watchdog_thread) in system_server()");
    if(pthread_join(monitor_thread_tid, NULL) == -1)
        perror("pthread_join(monitor_thread) in system_server()");
    if(pthread_join(disk_service_thread_tid, NULL) == -1)
        perror("pthread_join(disk_service_thread) in system_server()");
    if(pthread_join(camera_service_thread_tid, NULL) == -1)
        perror("pthread_join(camera_service_thread) in system_server()");

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

    return 0;
}
