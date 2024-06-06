#ifndef __MESSAGE_TYPE__
#define __MESSAGE_TYPE__
#include <mqueue.h>

#define MQ_FIN 9999

struct msg_t
{
	int type;
	int param1;
	int param2;
};

struct sensor_info_t
{
	double temper;
	double press;
	double humid;
};

struct connect_t my_connect(mqd_t mqd);
struct connect_t my_disconnect(mqd_t mqd);
int mq_4way_handshake(const char *name);
#endif
