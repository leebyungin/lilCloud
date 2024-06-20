#ifndef __MESSAGE_TYPE__
#define __MESSAGE_TYPE__
#include <mqueue.h>

enum MESSAGE_TYPE
{
	MESSAGE_TYPE_BASE = 9900,

	MQ_FIN = MESSAGE_TYPE_BASE,
	MQ_DUMP
};

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
