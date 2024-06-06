#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <execinfo.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <string.h>

#include <message.h>

//static void split_line(char *line, char *delim);

// stdout "Module_name [pid]: msg\n"
int pMessage(const char *msg, ...)
{
	char module_name[16];
	pid_t pid;
	va_list ap;
	int msg_len = strlen(msg);
	int escape_sequence = 0;

	prctl(PR_GET_NAME, (unsigned long)module_name);
	pid= getpid();

	printf("%s [%d]: ", module_name, pid);

	va_start(ap, msg);

	for(int i = 0; i < msg_len; i++)
	{
		if(msg[i] == '%')
		{
			escape_sequence = 1;
			continue;
		}
		else if(escape_sequence == 1)
		{
			escape_sequence = 0;
			switch(msg[i])
			{
				case 'd':
					printf("%d", va_arg(ap, int));
					break;
				case 's':
					printf("%s", va_arg(ap, char *));
					break;
				case 'f':
					printf("%0.2f", va_arg(ap, double));
					break;
				default :
					printf("\33[31m'%%%c' type is unimplemented in pMessage()\33[37m", msg[i]);
			}
			continue;
		}
		putchar(msg[i]);
	}
	putchar('\n');
}
//  stderr "msg [backtraces]"
void error_handler(const char* msg, int bt)
{
	fprintf(stderr, "\33[31m%s\33[37m\n",msg);
	if(bt)
	{
		backtrace_log();
	}

	exit(1);
}
//  stderr "perror(msg) [backtraces]"
void perror_handler(const char* msg, int bt)
{
	fprintf(stderr, "\33[31m");
	//to-do: pid 출력하게. 임시로 해둔 거는 msg를 두번 출력함
	pMessage(msg);
	perror(msg);
	fprintf(stderr, "\33[37m");
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
