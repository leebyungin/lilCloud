#include <stdio.h>
#include <sys/prctl.h>

#include <system_server.h>
#include <gui.h>
#include <input.h>
#include <web_server.h>

int system_server()
{
    printf("system_server 프로세스 동작중!\n");

    while (1)
    {
        sleep(1);
    }

    return 0;
}

int create_system_server()
{
    pid_t systemPid;
    const char *name = "system_server";

    printf("system_server 프로세스 생성시작\n");

    switch (systemPid = fork())
    {
    case -1:
        printf("fork() failed\n");
        break;
    case 0:
        if (prctl(PR_SET_NAME, (unsigned long)name) == -1)
        {
            printf("prctl() failed\n");
            exit(1);
        }
        system_server();
        break;
    default:
        break;
    }

    return systemPid;
}