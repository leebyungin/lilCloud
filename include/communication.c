#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <semaphore.h>
#include <errno.h>

#include <communication.h>
#include <message.h>

#define MQ_KEY_MAX 10
#define MAX_COUNT 3

static sem_t *sem;

struct pack
{
	int msg;
	int echo;
};

static int exist(const char *name, const char *dir)
{
	char filename[1024];
	int ret = 0;

	sprintf(filename, "%s%s", dir,name);

	if(sem_wait(sem) == -1)
	{
		perror_handler("[communication.c]: sem_wait()", 0);
	}
	// return 0 means the file exists
	if(access(filename, F_OK) == 0)
	{
		ret =1;
	}
	else
	{
		if(mq_open(name, O_CREAT | O_EXCL, S_IRWXU, NULL) == -1)
		{
			perror_handler("[communication.c]: mq_open()", 0);
		}
	}
	if(sem_post(sem) == -1)
	{
		perror_handler("[communication.c]: sem_post()", 0);
	}

	return ret;
}

static int sync_ack(const char *input_stream, const char *output_stream, const int host)
{
	struct pack data;
	int read_num;
	struct mq_attr attr;
	mqd_t input, output;
	int ret = 0;
	int saved_errno = errno;
	errno = 0;

	input = mq_open(input_stream, O_CREAT | O_RDWR, S_IRWXU);
	if(input == -1)
	{
		perror_handler(input_stream, 0);
	}
	output = mq_open(output_stream, O_CREAT | O_RDWR, S_IRWXU, NULL);
	if(output == -1)
	{
		perror_handler(output_stream, 0);
	}
	mq_getattr(input, &attr);

	for(int i = 0; i < MAX_COUNT; i++)
	{
		if(host)
		{
			data.msg = MQ_FIN;
			if(mq_send(output, (void*)&data, sizeof(struct pack), 0) == -1)
			{
				perror_handler("[communication.c]: mq_send(host->guest)", 0);
			}
			read_num = mq_receive(input, (void*)&data, attr.mq_msgsize, NULL);
			if(read_num == -1)
			{
				perror_handler("[communication.c]: mq_receive(guest->host)", 0);
			}

			if(data.echo == MQ_FIN)
			{
				ret = 1;
				break;
			}
		}
		else
		{
			read_num = mq_receive(input, (void*)&data, attr.mq_msgsize, NULL);
			if(read_num == -1)
			{
				perror_handler("[communication.c]: mq_receive(host->guest)", 0);
			}

			if(data.msg == MQ_FIN)
			{
				data.echo = data.msg;
				data.msg = 0;

				if(mq_send(output, (void*)&data, sizeof(struct pack), 0) == -1)
				{
					perror_handler("[communication.c]: mq_send(guest->host)", 0);
				}

				ret = 1;
				break;
			}
		}
	}
	
	if(mq_close(input) == -1)
	{
		perror_handler("[communication.c]: mq_close()", 0);
	}
	if(mq_close(output) == -1)
	{
		perror_handler("[communication.c]: mq_close()", 0);
	}

	if(mq_unlink(input_stream) == -1)
	{
		if(errno != ENOENT)
		{
			perror_handler("[communication.c]: [communication.c]: mq_unlink()1", 0);
		}
		errno = 0;
	}
	if(mq_unlink(output_stream) == -1)
	{
		if(errno != ENOENT)
		{
			perror_handler("[communication.c]: [communication.c]: mq_unlink()2", 0);
		}
		errno = 0;
	}
	
	errno = saved_errno;
	return 1;
}

int mq_4way_handshake(const char* name)
{
	int str_len;
	const char *dir = "/dev/mqueue";
	char host_name[1024] = {0,};
	char guest_name[1024] = {0,};
	int ret = 0;	
	int saved_errno = errno;
	errno = 0;

	sem = sem_open(name, O_CREAT, S_IRWXU, 1);
	if(sem == SEM_FAILED)
	{
		perror_handler("[communication.c]: sem_open():", 0);
	}

	sprintf(host_name, "%s_host", name);
	sprintf(guest_name, "%s_guest", name);

	if(exist(host_name, dir))
	{
		// *name already Exist.
		// You are a guest.
		ret = sync_ack(guest_name, host_name, 0);
	}
	else
	{
		// *name doesn't Exisxt.
		// You are a host.
		ret = sync_ack(host_name, guest_name, 1);
	}

	if(sem_close(sem) == -1)
	{
		perror_handler("[communication.c]: sem_close()", 0);
	}
	errno = 0;
	if(sem_unlink(name) == -1)
	{
		if(errno != ENOENT)
		{
			perror_handler("[communication.c]: sem_unlink()", 0);
		}
	}

	errno = saved_errno;
	return ret;
}
