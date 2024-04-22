#include <stdio.h>
#include <unistd.h>

#include <system_server.h>
#include <gui.h>
#include <input.h>
#include <web_server.h>
#include <message.h>
extern char ** environ;

static const char *moduleName= "web_server";
static pid_t globalPid= -1;

int create_web_server()
{
    char *pathName= "/usr/local/bin/filebrowser";
    char *args[]= {"filebrowser", "-p", "8282", NULL};

    globalPid= getpid();
    pMessage(moduleName, globalPid, "Starting");

    switch (globalPid= fork())
    {
    case -1:
        pMessage(moduleName, globalPid, "fork() failed");
        exit(1);
        break;
    case 0:
        globalPid= getpid();

        pMessage(moduleName, globalPid, "exec(filebrowser)");
        if (execve(pathName, args, environ) == -1)
        {
            pMessage(moduleName, globalPid, "execve() failed");
            exit(1);
        }
        break;
    default:
        break;
    }

    return globalPid;
}
