#include <stdio.h>
#include <unistd.h>

#include <system_server.h>
#include <gui.h>
#include <input.h>
#include <web_server.h>
extern char ** environ;

int create_web_server()
{
    pid_t systemPid;
    char *pathName = "/usr/local/bin/filebrowser";
    char *args[] = {"filebrowser", "-p", "8282", NULL};

    printf("web_server 프로세스 생성시작\n");

    switch (systemPid = fork())
    {
    case -1:
        printf("fork() failed\n");
        exit(1);
        break;
    case 0:
        if (execve(pathName, args, environ) == -1)
        {
            printf("execl() failed\n");
            exit(1);
        }
        break;
    default:
        break;
    }

    return systemPid;
}