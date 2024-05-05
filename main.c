#include <stdio.h>
#include <sys/wait.h>
#include <signal.h>

#include <system_server.h>
#include <gui.h>
#include <input.h>
#include <web_server.h>
#include <message.h>

void sigchldHandler(int signal);

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
	pMessage("call system_server()");
	spid = create_system_server();
	processCount++;
	
	pMessage("call web_server()");
	wpid = create_web_server();
	processCount++;
	
	pMessage("call input()");
	ipid = create_input();
	processCount++;
	
	pMessage("call gui()");
	gpid = create_gui();
	processCount++;
	

	// issue: 현재 방식은 프로세스의 종료 순서가 고정돼 있음.
	/*
	waitpid(spid, &status, 0);
	waitpid(gpid, &status, 0);
	waitpid(ipid, &status, 0);
	waitpid(wpid, &status, 0);
	*/

	while (processCount > 0)
	{
		pause();
	};

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
