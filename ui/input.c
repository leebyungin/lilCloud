#include <stdio.h>
#include <sys/prctl.h>
#include <execinfo.h>
#include <signal.h>
#include <pthread.h>

#include <system_server.h>
#include <gui.h>
#include <input.h>
#include <web_server.h>
#include <message.h>

// about signal
static void regist_signal_handler(int signum, void (*handler)(int));
static void segfault_handler(int signal);

// about thread
static int create_pthread(pthread_t *tid, void *(*start_routine)(void *), void *arg);
static void *command(void*);
static void *sensor(void*);
static void _command(void);	

static const char *moduleName= "input";
static pid_t globalPid= -1;

int input()
{
    struct sigaction sa;
	pthread_t commandTid, sensorThread;

    pMessage("Running");

	regist_signal_handler(SIGSEGV, segfault_handler);

	create_pthread(&commandTid, command, NULL);
	create_pthread(&sensorThread, sensor, NULL);

    while (1)
    {
        sleep(1);
    }

    return 0;
}

static void segfault_handler(int signal)
{
    backtrace_log();
}

int create_input()
{
    globalPid= getpid();

    switch (globalPid = fork())
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

        input();

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
    sa.sa_handler = segfault_handler;

    sigaction(signum, &sa, NULL);
}
static int create_pthread(pthread_t *tid, void *(*start_routine)(void *), void *arg)
{
	int s;
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	s = pthread_create(tid, &attr, start_routine, arg);
	pthread_attr_destroy(&attr);

	return s;
}
static void *command(void*)
{
	pMessage("Command thread running");
	_command();	

	return NULL;
}
static void *sensor(void*)
{
	pMessage("sensor thread running");
	while(1)
	{
		sleep(1);
	}

	return NULL;
}
static void _command(void)
{
	while(1)
	{
		sleep(1);
	}
}
