#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#ifdef __cplusplus
extern "C" {
#endif

#define ERROR_BUF_SIZE 15

int pMessage(const char *msg, ...);
void error_handler(const char* msg, int bt);
void perror_handler(const char* msg, int bt);
void backtrace_log(void);

#ifdef __cplusplus
}
#endif

#endif
