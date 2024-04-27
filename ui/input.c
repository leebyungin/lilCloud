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

    pMessage("Running");

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = segfault_handler;

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

void segfault_handler(int signal)
{
    backtrace_log();
}

int create_input()
{
    globalPid= getpid();

    switch (globalPid = fork())
    {
    case -1:
        pMessage("fork() failed");
        break;
    case 0:
        globalPid= getpid();

        if (prctl(PR_SET_NAME, (unsigned long)moduleName) == -1)
        {
            pMessage("prctl() failed");
            exit(1);
        }

        input();

        pMessage("Done");
        exit(0);
        break;
    default:
        break;
    }

    return globalPid;
}
