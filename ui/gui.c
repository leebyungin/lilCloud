#include <stdio.h>
#include <unistd.h>
#include <sys/prctl.h>

#include <system_server.h>
#include <gui.h>
#include <input.h>
#include <web_server.h>
#include <message.h>
extern char **environ;

static const char *moduleName= "gui";
static pid_t globalPid= -1;

int create_gui()
{
    char *pathName= "/usr/bin/google-chrome-stable";
    char *args[]= {pathName, "--new-window","http://localhost:8282", NULL};

    globalPid= getpid();

    sleep(3);
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

        pMessage("exec(google-chrome-stable)");
        if (execve(pathName, args, environ) == -1)
        {
            pMessage("execve() failed");
            exit(1);
        }

        exit(0);
        break;
    default:
        break;
    }

    return globalPid;
}