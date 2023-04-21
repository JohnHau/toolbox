#ifndef __MSG_QUEUE_H__
#define __MSG_QUEUE_H__

typedef struct mqueue
{
	long type;
	uint32_t device_id;
	float temperature;
	float moisture;
	int8_t year;
	int8_t month;
	int8_t day;
	int8_t hour;
	int8_t minute;
	int8_t second;

}MQUEUE;


#endif
