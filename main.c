#include <stdio.h>
#include <sys/wait.h>
#include <signal.h>

#include <system_server.h>
#include <gui.h>
#include <input.h>
#include <web_server.h>
#include <message.h>

void sigchldHandler(int signal);

static const char *moduleName= "main";
static pid_t globalPid=-1;

int main()
{
    pid_t spid, gpid, ipid, wpid;
    int status, savedErrno;
    struct sigaction sa;

    globalPid= getpid();

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = sigchldHandler;
    if(sigaction(SIGCHLD, &sa, NULL) == -1)
    {
        pMessage("sigaction() failed");
        return -1;
    }

    pMessage("Starting");
    pMessage("call system_server()");
    spid = create_system_server();
    pMessage("call web_server()");
    wpid = create_web_server();
    pMessage("call input()");
    ipid = create_input();
    pMessage("call gui()");
    gpid = create_gui();

    waitpid(spid, &status, 0);
    waitpid(gpid, &status, 0);
    waitpid(ipid, &status, 0);
    waitpid(wpid, &status, 0);

    return 0;
}

void sigchldHandler(int signal)
{
    pid_t childPid;
    int status;

    childPid= waitpid(-1, &status, WNOHANG);
    printf("main.handler : Caught SIGCHLD(%d)\n",signal);
    //pMessage(moduleName, globalPid, "Caught SIGCHLD(%d)",signal);
    printf("main.handler : Reaped chld(%d)\n",childPid);
    //pMessage(moduleName, globalPid, "Reaped child(%d)",childPid);
}
