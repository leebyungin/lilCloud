#include <stdio.h>
#include <sys/wait.h>
#include <signal.h>
#include <mqueue.h>

#include <system_server.h>
#include <gui.h>
#include <input.h>
#include <web_server.h>
#include <message.h>

static void sigchldHandler(int signal);

// 메세지 큐 처리 함수
static void mq_create(void);
static void mq_destroy(void);
static int mq_count(void);

static const char *moduleName = "main";
static pid_t globalPid = -1;

static volatile int processCount = 0;

int main()
{
	pid_t spid, gpid, ipid, wpid;
	int status, savedErrno;
	struct sigaction sa;

	globalPid = getpid();

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = sigchldHandler;
	if (sigaction(SIGCHLD, &sa, NULL) == -1)
	{
		pMessage("sigaction() failed");
		return -1;
	}

	pMessage("Starting");
	mq_destroy();
	mq_create();

	pMessage("call system_server()");
	spid = create_system_server();
	processCount++;
	/*
	pMessage("call web_server()");
	wpid = create_web_server();
	processCount++;
	*/
	pMessage("call input()");
	ipid = create_input();
	processCount++;
	/*
	pMessage("call gui()");
	gpid = create_gui();
	processCount++;
	*/

	// issue: 현재 방식은 프로세스의 종료 순서가 고정돼 있음.
	/*
	waitpid(spid, &status, 0);
	waitpid(gpid, &status, 0);
	waitpid(ipid, &status, 0);
	waitpid(wpid, &status, 0);
	*/

	while (processCount > 0)
	{
		pMessage("Remain process (%d)", processCount);
		pause();
	};

	mq_destroy();

	return 0;
}

void sigchldHandler(int signal)
{
	pid_t childPid;
	int status;
	int savederrno = errno = 0;

	childPid = waitpid(-1, &status, WNOHANG);
	switch (childPid)
	{
	case -1:
		if (errno != ECHILD)
		{
			perror_handler("sigchldHandler()", 0);
		}
		break;
	case 0:
		perror_handler("Child not changed. but SIGCHLD?", 0);
		break;
	default:
		pMessage("Caught SIGCHLD(%d)", signal);
		pMessage("Reaped child(%d)", childPid);
		break;
	}
	processCount--;

	errno = savederrno;
}

static const char * mq_names[] =
{
	"/watchdog_mq",
	"/monitor_mq",
	"/disk_mq",
	"/camera_mq",
};


static int mq_count(void)
{
	return sizeof(mq_names) / sizeof(char *);
}

static mqd_t mymq_open(const char *name)
{
	int mq_flag = O_RDWR | O_CREAT | O_EXCL;
	mode_t mode = S_IRUSR | S_IWUSR;
	return mq_open(name, mq_flag, mode, NULL);
}

static void mq_create(void)
{
	int saved_error = errno;
	errno = 0;

	for(int i = 0; i < mq_count(); i++)
	{
		if(mymq_open(mq_names[i]) == -1)
		{
			pMessage("Open mq *FAIL* (%s)", mq_names[i]);
			perror_handler(mq_names[i], 1);
		}
		else
		{
			pMessage("Open mq (%s)", mq_names[i]);
		}
	}

	errno = saved_error;
	return;
}

static void mq_destroy(void)
{
	int saved_errno = errno;
	errno = 0;
	int temp = 0;

	for(int i = 0; i < mq_count(); i++)
	{
		temp = mq_unlink(mq_names[i]);
		if(temp == -1 && errno != ENOENT)
		{
			pMessage("Unlink mq *FAIL* (%s)", mq_names[i]);
			perror_handler(mq_names[i], 0);
		}
		else
		{
			pMessage("Unlink mq (%s)", mq_names[i]);
		}
	}

	errno = saved_errno;
	return;
}
