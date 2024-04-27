#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <execinfo.h>
#include <unistd.h>
#include <sys/prctl.h>

#include <message.h>

int pMessage(const char *message, ...)
{
    char module_name[16];
    pid_t pid;

    prctl(PR_GET_NAME, (unsigned long)module_name);
    pid= getpid();

    printf("%s [%d]: %s\n", module_name, pid, message);
}
void error_handler(const char* msg, int bt)
{
    fprintf(stderr, "%s\n",msg);
    if(bt)
    {
        backtrace_log();
    }
    exit(1);
}
void perror_handler(const char* msg, int bt)
{
    perror(msg);
    if(bt)
    {
        backtrace_log();
    }
    exit(1);
}
void backtrace_log(void)
{
    void *buf[ERROR_BUF_SIZE];
    int size;
    
    size= backtrace(buf, ERROR_BUF_SIZE);
    backtrace_symbols_fd(buf, size, STDERR_FILENO);
}
