#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#define ERROR_BUF_SIZE 15

int pMessage(const char *name, pid_t pid, const char *message, ...);
void error_handler(const char* msg, int bt);
void perror_handler(const char* msg, int bt);
void backtrace_log(void);
#endif
