#include <stdio.h>
#include <sys/prctl.h>
#include <sys/wait.h>
#include <execinfo.h>
#include <signal.h>
#include <pthread.h>

#include <system_server.h>
#include <gui.h>
#include <input.h>
#include <web_server.h>
#include <message.h>

// #define _DEBUG_

// about signal
static void regist_signal_handler(int signum, void (*handler)(int));
static void segfault_handler(int signal);

// about thread
static int create_pthread(pthread_t *tid, void *(*start_routine)(void *), void *arg);
static void *command(void *);
static void *sensor(void *);

// about command
static void command_loop(void);
static char **parse_line(char *line, char *delim);
static int execute(char **args);
static int my_shell(char **args);
static int my_send(char **args);
static int my_exit(char **args);
static char *builtin_command[] =
    {
        "sh",
        "send",
        "exit"};
static int builtin_count()
{
    return sizeof(builtin_command) / sizeof(char *);
}
static int (*builtin_func[])(char **arg) =
    {
        &my_shell,
        &my_send,
        &my_exit};

static void print_vector(const char **arr);
static const char *moduleName = "input";

int input()
{
    struct sigaction sa;
    pthread_t commandTid, sensorTid;

    pMessage("Running");

    regist_signal_handler(SIGSEGV, segfault_handler);

    create_pthread(&sensorTid, sensor, NULL);
    create_pthread(&commandTid, command, NULL);

    pthread_join(commandTid, NULL);
    pMessage("Command thread terminated");
    pthread_join(sensorTid, NULL);
    pMessage("Sensor thread terminated");

    while (1)
    {
        sleep(1);
    }

    return 0;
}

static void segfault_handler(int signal)
{
    backtrace_log();
}

int create_input()
{
    pid_t pid;
    pid = getpid();

    switch (pid = fork())
    {
    case -1:
        pMessage("fork() failed");
        break;
    case 0:
        pid = getpid();

        if (prctl(PR_SET_NAME, (unsigned long)moduleName) == -1)
        {
            pMessage("prctl() failed");
            exit(1);
        }

        input();

        exit(0);
        break;
    default:
        break;
    }

    return pid;
}
void regist_signal_handler(int signum, void (*handler)(int))
{
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = segfault_handler;

    sigaction(signum, &sa, NULL);
}
static int create_pthread(pthread_t *tid, void *(*start_routine)(void *), void *arg)
{
    int s;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    s = pthread_create(tid, &attr, start_routine, arg);
    pthread_attr_destroy(&attr);

    return s;
}
static void *command(void *)
{
    pMessage("Command thread running");

    command_loop();

    return NULL;
}
static void *sensor(void *)
{
    pMessage("Sensor thread running");
    while (1)
    {
        sleep(1);
    }

    return NULL;
}
static void command_loop(void)
{
    int savederrno = errno;
    int status = 1;
    size_t line_len = 0;
    char *line = NULL;
    char **tokens = NULL;
    int s;

    while (status)
    {
        printf("\33[32mlilCloud\33[37m> ");

        if (getline(&line, &line_len, stdin) == -1)
        {
            // ctrl + d 입력
            if (errno == 0)
            {
                pMessage("EOT");
                break;
            }
            else
            {
                perror_handler("input.command_loop.getline()", 0);
            }
        }
        tokens = parse_line(line, " \t\n");
#ifdef _DEBUG_
        print_vector((const char **)tokens);
#endif
        status = execute(tokens);
    }

    free(tokens);
    free(line);

    errno = savederrno;
    return;
}

static char **parse_line(char *line, char *delim)
{
    int savedErrno = errno;
    errno = 0;
    char *token = NULL;
    char **tokens = NULL;
    unsigned int position = 0;
    unsigned int bufsize = ARG_MAX;

    tokens = (char **)malloc(bufsize * sizeof(char *));
    if (tokens == NULL)
    {
        perror_handler("input.parse_line.malloc()", 0);
    }

    token = strtok(line, delim);
    while (token != NULL)
    {
        tokens[position++] = token;

        if (bufsize <= position)
        {
            bufsize += ARG_MAX;
            if (realloc(tokens, bufsize * sizeof(char *)) == NULL)
            {
                perror_handler("input.parse_line()", 0);
            }

            for (unsigned int i = position; i < bufsize; i++)
            {
                // issue: 초기화 확인해보기
                tokens[i] == NULL;
            }
        }

        token = strtok(NULL, delim);
    }

    tokens[position] = NULL;

    errno = savedErrno;

    return tokens;
}

static int execute(char **args)
{
    int retval = -1;

    if (args[0] == NULL)
    {
        return 1;
    }

    for (int i = 0; i < builtin_count(); i++)
    {
        if (strcmp(args[0], builtin_command[i]) == 0)
        {
            retval = builtin_func[i](args);
            break;
        }
    }
    if (retval == -1)
    {
        printf("%s: command not fonund\n", args[0]);
    }

    return retval;
}

static int my_shell(char **args)
{
    pid_t pid;
    int retval = 0;

    // issue: 프로세스 종료 제대로 안됨
    switch (pid = fork())
    {
    case -1:
        perror_handler("input.my_shell.fork()", 0);
        break;
    case 0:
        prctl(PR_SET_NAME, args[0]);
        if (execvp(args[0], args) == -1)
        {
            perror_handler("input.my_shell.exevp()", 0);
        }
        break;
    default:
        retval = 1;
        pMessage("Wait child(%d)", pid);
        waitpid(pid, NULL, 0);
        pMessage("Reaped pid(%d)", pid);
        break;
    }

    return retval;
}
static int my_send(char **args)
{
}
static int my_exit(char **args)
{
    return 0;
}

static void print_vector(const char **arr)
{
    unsigned int i = 0;

    pMessage("Print vector");
    while (arr[i] != NULL)
    {
        pMessage(arr[i]);
        i++;
    }
}
