#include <stdio.h>
#include <sys/prctl.h>
#include <execinfo.h>
#include <signal.h>

#include <system_server.h>
#include <gui.h>
#include <input.h>
#include <web_server.h>
#include <message.h>

static const char *moduleName= "input";
static pid_t globalPid= -1;

int input()
{
    struct sigaction sa;

    pMessage(moduleName, globalPid, "Running");

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = sigSegvHandler;

    if( sigaction(SIGSEGV, &sa, NULL) == -1)
    {
        pMessage(moduleName, globalPid, "sigaction() failed");
    }

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
        pMessage(moduleName, globalPid, "backtrace_symbols() failed");
    }

    for (int i = 0; i < size; i++)
    {
        printf("[bt]\t%s\n", symbols[i]);
    }

    free(symbols);
    exit(1);
}

int create_input()
{
    globalPid= getpid();
    pMessage(moduleName, globalPid, "Starting");

    switch (globalPid = fork())
    {
    case -1:
        pMessage(moduleName, globalPid, "fork() failed");
        break;
    case 0:
        globalPid= getpid();

        if (prctl(PR_SET_NAME, (unsigned long)moduleName) == -1)
        {
            pMessage(moduleName, globalPid, "prctl() failed");
            exit(1);
        }

        input();

        pMessage(moduleName, globalPid, "Done");
        exit(0);
        break;
    default:
        break;
    }

    return globalPid;
}
