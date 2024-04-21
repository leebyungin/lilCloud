#include <stdio.h>
#include <sys/prctl.h>
#include <execinfo.h>
#include <signal.h>

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

void sigSegvHandler(int signal)
{
    void *buffer[BUF_SIZE];
    char **symbols;
    int size = BUF_SIZE;

    size = backtrace(buffer, size);

    symbols = backtrace_symbols(buffer, size);
    if (symbols == NULL)
    {
        printf("backtrace_symbols() failed!\n");
    }

    for (int i = 0; i < size; i++)
    {
        printf("\t%s\n", symbols[i]);
    }

    free(symbols);
}

int create_input()
{
    pid_t systemPid;
    const char *name = "input";
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = sigSegvHandler;
    if( sigaction(SIGSEGV, &sa, NULL) == -1)
    {
        printf("sigaction() failed\n");
    }

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
