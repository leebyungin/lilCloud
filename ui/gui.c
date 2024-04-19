#include <stdio.h>
#include <unistd.h>

#include <system_server.h>
#include <gui.h>
#include <input.h>
#include <web_server.h>
extern char **environ;

int create_gui()
{
    pid_t systemPid;
    char *pathName = "/usr/bin/google-chrome-stable";
    char *args[] = {pathName, "http://localhost:8282", NULL};

    printf("gui 프로세스 생성 시작\n");

    sleep(3);
    switch (systemPid = fork())
    {
    case -1:
        printf("fork() failed!\n");
        break;
    case 0:
        if (execve(pathName,args,environ) == -1)
        {
            printf("execl() failed!\n");
            exit(1);
        }
        break;
    default:
        break;
    }

    return systemPid;
}
