#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <message.h>

int pMessage(const char *name, pid_t pid, const char *message, ...)
{
    printf("%s [%d]: %s\n", name, pid, message);
}