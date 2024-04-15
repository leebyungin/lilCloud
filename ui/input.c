#include <stdio.h>
#include <sys/prctl.h>

#include <system_server.h>
#include <gui.h>
#include <input.h>
#include <web_server.h>

int input()
{
    printf("input 프로세스 동작중!\n");

    while (1)
    {
        sleep(1);
    }

    return 0;
}

int create_input()
{
    pid_t systemPid;
    const char *name = "input";

    printf("input 프로세스 생성시작\n");

    switch (systemPid = fork())
    {
    case -1:
        printf("fork() failed");
        break;
    case 0:
        if (prctl(PR_SET_NAME, (unsigned long)name) == -1)
        {
            printf("prctl() failed\n");
            exit(1);
        }
        input();
        break;
    default:
        break;
    }

    return systemPid;
}
