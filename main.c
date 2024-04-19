#include <stdio.h>
#include <sys/wait.h>
#include <signal.h>

#include <system_server.h>
#include <gui.h>
#include <input.h>
#include <web_server.h>

void sigchldHandler(int signal);

int main()
{
    pid_t spid, gpid, ipid, wpid;
    int status, savedErrno;
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = sigchldHandler;
    if(sigaction(SIGCHLD, &sa, NULL) == -1)
    {
        printf("sigaction() failed\n");
    }

    printf("main() 프로세스 시작\n");
    printf("sytem_server() 호출\n");
    spid = create_system_server();
    printf("web_server() 호출\n");
    wpid = create_web_server();
    printf("input() 호출\n");
    ipid = create_input();
    printf("gui() 호출\n");
    gpid = create_gui();

    waitpid(spid, &status, 0);
    waitpid(gpid, &status, 0);
    waitpid(ipid, &status, 0);
    waitpid(wpid, &status, 0);

    return 0;
}

void sigchldHandler(int signal)
{
    printf("main: Caught SIGCHLD:%d\n",signal);
}
