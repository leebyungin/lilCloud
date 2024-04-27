#include <stdio.h>
#include <sys/prctl.h>

#include <system_server.h>
#include <gui.h>
#include <input.h>
#include <web_server.h>
#include <message.h>

static const char *moduleName = "system_server";
static pid_t globalPid= -1;

int system_server()
{
    pMessage("Running");
    while (1)
    {
        sleep(1);
    }

    return 0;
}

int create_system_server()
{
    globalPid= getpid();

    switch (globalPid= fork())
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

        system_server();

        pMessage("Done");
        exit(0);
        break;
    default:
        break;
    }

    return globalPid;
}
