#ifndef __MESSAGE_TYPE__
#define __MESSAGE_TYPE__

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

#endif
