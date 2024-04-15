#include <stdio.h>
#include <sys/wait.h>

#include <system_server.h>
#include <gui.h>
#include <input.h>
#include <web_server.h>

int main()
{
    pid_t spid, gpid, ipid, wpid;
    int status, savedErrno;

    printf("main() 프로세스 시작\n");
    printf("sytem_server() 호출\n");
    spid = create_system_server();
    printf("web_server() 호출\n");
    wpid = create_web_server();
    printf("input() 호출\n");
    ipid = create_input();
    printf("gui() 호출\n");
    gpid = create_gui();

    waitpid(spid, &status, 0);
    waitpid(gpid, &status, 0);
    waitpid(ipid, &status, 0);
    waitpid(wpid, &status, 0);

    return 0;
}
