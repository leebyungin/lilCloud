#include <stdio.h>
#include <sys/prctl.h>
#include <sys/time.h>  //setitimer
#include <signal.h>
#include <pthread.h>
#include <mqueue.h>
#include <semaphore.h>

#include <system_server.h>
#include <gui.h>
#include <input.h>
#include <web_server.h>
#include <message.h>
#include <message_type.h>

#define TIMER_SEM "/timerSem"

#define WATCHDOG_MQ "/watchdog_mq"
#define MONITOR_MQ "/monitor_mq"
#define DISK_MQ "/disk_mq"
#define CAMERA_MQ "/camera_mq"

static sem_t *timer_sem;

// signal 관련
static void regist_signal_handler(int signum, void (*handler)(int));
static void create_periodic_timer(unsigned int sec, unsigned int usec);
static void timer_handler(int sig);

// thread 관련
static int create_pthread(pthread_t *tid, void *(*start_routine)(void*), void *arg);
static void *watchdog(void *);
static void *disk_service(void *);
static void *monitor(void *);
static void *camera_service(void *);
static void *timer_service(void *);

static const char *moduleName = "system_server";
static unsigned int alarm_count = 0;
static pid_t globalPid= -1;

int system_server()
{
	pthread_t watchdogTid, diskServiceTid, monitorTid, cameraServiceTid, timerServiceTid;

    pMessage("Running");



	
	create_pthread(&watchdogTid, watchdog, NULL);
	create_pthread(&diskServiceTid, disk_service, NULL);
	create_pthread(&monitorTid, monitor, NULL);
	create_pthread(&cameraServiceTid, camera_service, NULL);
	create_pthread(&timerServiceTid, timer_service, NULL);

    while (1)
    {
        sleep(1);
    }

	if(sem_unlink(TIMER_SEM) == -1)
	{
		perror_handler("system_server(): sem_unllink() *FAIL*", 0);
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
	if(sem_post(timer_sem) == -1)
	{
		perror_handler("timer_handler: sem_post() *FAIL*", 0);
	}
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
	mqd_t watchdog_mq; 
	struct mq_attr attr;
	char *buff;
	int msgsize;

	pMessage("Watchdog thread running");

	if((watchdog_mq = mq_open(WATCHDOG_MQ, O_RDONLY)) == -1)
	{
		perror_handler("[watchdog thread]: mq_open() *FAIL*", 0);
	}
	

	if(mq_getattr(watchdog_mq, &attr) == -1)
	{
		perror_handler("[watchdog thread]: mq_getattr() *FAIL*", 0);
	}
	msgsize = attr.mq_msgsize;
	buff = (char*)malloc(sizeof(char) * msgsize);

	while(1)
	{
		if(mq_receive(watchdog_mq, buff, msgsize, NULL) == -1)
		{
			perror_handler("[watchdog thread]: mq_receive() *FAIL*", 0);
		}

		pMessage("[WATCHDOG]: %s", buff);
	}

	free(buff);
	if(mq_close(watchdog_mq) == -1)
	{
		perror_handler("[watchdog thread]: mq_close() *FAIL*", 0);
	}
	return NULL;
}
void *disk_service(void *)
{
	mqd_t disk_mq;
	struct mq_attr attr;
	char *buff;
	int msgsize;

	pMessage("disk_service thread running");

	if((disk_mq = mq_open(DISK_MQ, O_RDONLY)) == -1)
	{
		perror_handler("[disk_service thread]: mq_open() *FAIL*", 0);
	}
	
	if(mq_getattr(disk_mq, &attr) == -1)
	{
		perror_handler("[disk_service thread]: mq_getattr() *FAIL*", 0);
	}
	msgsize = attr.mq_msgsize;
	buff = (char*)malloc(sizeof(char) * msgsize);

	while(1)
	{
		FILE *fp;
		char buf[1024];
		int str_len;

		if(mq_receive(disk_mq, buff, msgsize, NULL) == -1)
		{
			perror_handler("[disk_service thread]: mq_receive() *FAIL*", 0);
		}
		
		fp = popen("df","r");
		while(fgets(buf, sizeof(buf), fp) != NULL)
		{
			write(STDOUT_FILENO, buf, sizeof(buf)/sizeof(char));
		}
		pclose(fp);	
	}

	free(buff);
	if(mq_close(disk_mq) == -1)
	{
		perror_handler("[disk_service thread]: mq_close() *FAIL*", 0);
	}
	return NULL;
}
void *monitor(void *)
{
	mqd_t monitor_mq;
	struct mq_attr attr;
	struct msg_t msg;
	int msgsize;

	pMessage("Monitor thread running");

	if((monitor_mq = mq_open(MONITOR_MQ, O_RDONLY)) == -1)
	{
		perror_handler("[monitor thread]: mq_open() *FAIL*", 0);
	}
	
	if(mq_getattr(monitor_mq, &attr) == -1)
	{
		perror_handler("[monitor thread]: mq_getatttr() *FAIL*", 0);
	}

	msgsize = attr.mq_msgsize;

	while(1)
	{
		if(mq_receive(monitor_mq, (void*)&msg, msgsize, NULL) == -1)
		{
			perror_handler("[monitor tread]: mq_receive() *FAIL*", 0);
		}

		pMessage("[Monitor thread]:Message type : %d", msg.type);
		pMessage("[Monitor thread]:Message param1: %d", msg.param1);
		pMessage("[Monitor thread]:Message param2: %d", msg.param2);
	}

	if(mq_close(monitor_mq) == -1)
	{
		perror_handler("[monitor thread]: mq_close() *FAIL*", 0);
	}
	return NULL;
}

void *camera_service(void *)
{
	mqd_t camera_mq;
	struct mq_attr attr;
	char *buff;
	int msgsize;

	pMessage("Camera_service thread running");

	if((camera_mq = mq_open(CAMERA_MQ, O_RDONLY)) == -1)
	{
		perror_handler("[camera_service thread]: mq_open() *FAIL*", 0);
	}
	
	if(mq_getattr(camera_mq, &attr) == -1)
	{
		perror_handler("[camera_service thread]: mq_getatttr() *FAIL*", 0);
	}
	msgsize = attr.mq_msgsize;
	buff = (char*)malloc(sizeof(char) * msgsize);

	while(1)
	{
		if(mq_receive(camera_mq, buff, msgsize, NULL) == -1)
		{
			perror_handler("[camera_service thread]: mq_receive() *FAIL*", 0);
		}
	}

	free(buff);
	if(mq_close(camera_mq) == -1)
	{
		perror_handler("[camera_service thread]: mq_close() *FAIL*", 0);
	}

	return NULL;
}

void *timer_service(void *)
{
	if(sem_unlink(TIMER_SEM) == -1)
	{
		if(errno != ENOENT)
		{
			perror_handler("system_server(): sem_unlink() *FAIL*", 0);
		}
	}

	if((timer_sem = sem_open(TIMER_SEM, O_CREAT | O_EXCL, S_IWUSR | S_IRUSR, 0)) == SEM_FAILED)
	{
		perror_handler("systme_server(): sem_open() *FAIL*", 0);
	}

	regist_signal_handler(SIGALRM, timer_handler);
	create_periodic_timer(5, 0);

	while(1)
	{
		if(sem_wait(timer_sem) == -1)
		{
			perror_handler("timer_service: sem_wait() *FAIL*", 0);
		}

		alarm_count ++;
		printf("timer: %d\n", alarm_count);
	}
}
