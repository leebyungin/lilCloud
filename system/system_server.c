#include <stdio.h>
#include <sys/prctl.h>
#include <sys/time.h>  //setitimer
#include <signal.h>
#include <pthread.h>

#include <system_server.h>
#include <gui.h>
#include <input.h>
#include <web_server.h>
#include <message.h>

// about signal
static void regist_signal_handler(int signum, void (*handler)(int));
static void create_periodic_timer(unsigned int sec, unsigned int usec);
static void timer_handler(int sig);

// about thread
static int create_pthread(pthread_t *tid, void *(*start_routine)(void*), void *arg);
static void *watchdog(void *);
static void *disk_service(void *);
static void *monitor(void *);
static void *camera_service(void *);

static const char *moduleName = "system_server";
static unsigned int alarm_count = 0;
static pid_t globalPid= -1;

int system_server()
{
    struct sigaction sa;
	pthread_t watchdogTid, diskServiceTid, monitorTid, cameraServiceTid;

    pMessage("Running");

	regist_signal_handler(SIGALRM, timer_handler);
	create_periodic_timer(5, 0);

	create_pthread(&watchdogTid, watchdog, NULL);
	create_pthread(&diskServiceTid, disk_service, NULL);
	create_pthread(&monitorTid, monitor, NULL);
	create_pthread(&cameraServiceTid, camera_service, NULL);

    while (1)
    {
        sleep(1);
    }

    return 0;
}

int create_system_server()
{
    globalPid= getpid();

    switch (globalPid= fork())
    {
    case -1:
        pMessage("fork() failed");
        break;
    case 0:
        globalPid= getpid();

        if (prctl(PR_SET_NAME, (unsigned long)moduleName) == -1)
        {
            pMessage("prctl() failed");
            exit(1);
        }

        system_server();

        pMessage("Done");
        exit(0);
        break;
    default:
        break;
    }

    return globalPid;
}
void regist_signal_handler(int signum, void (*handler)(int))
{
	struct sigaction sa;
	
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = handler;

	sigaction(signum, &sa, NULL);
}
void create_periodic_timer(unsigned int sec, unsigned int usec)
{
	struct itimerval it;

	it.it_value.tv_sec = sec;
	it.it_value.tv_usec = usec;

	it.it_interval.tv_sec = sec;
	it.it_interval.tv_usec = usec;

	setitimer(ITIMER_REAL, &it, NULL);
}
void timer_handler(int sig)
{
    printf("timer_expire_signal_handler: %d\n", alarm_count++);
}
int create_pthread(pthread_t *tid, void *(*start_routine)(void*), void *arg)
{
	int s;
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	s = pthread_create(tid, &attr, start_routine, arg);
	pthread_attr_destroy(&attr);

	return s;
}
void *watchdog(void *)
{
	pMessage("Watchdog thread running");
	while(1)
	{
		sleep(1);
	}
	return NULL;
}
void *disk_service(void *)
{
	pMessage("Disk_service thread running");
	while(1)
	{
		sleep(1);
	}
	return NULL;
}
void *monitor(void *)
{
	pMessage("Monitor thread running");
	while(1)
	{
		sleep(1);
	}
	return NULL;
}
void *camera_service(void *)
{
	pMessage("Camera_service thread running");
	while(1)
	{
		sleep(1);
	}
	return NULL;
}
