#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include "msg_queue.h"

MQUEUE msq;

int main(int argc,char* argv[])
{
	int msqid = 0;
	key_t mkey = 0x9889;

	if((msqid = msgget(mkey,0777)) < 0 )
	{
		perror("msgget failed\n");
		exit(EXIT_FAILURE);
	}


	msq.type = 1;
	msq.temperature = 25.0;
	msq.moisture = 65.0;

	if(msgrcv(msqid,&msq,sizeof(msq)-sizeof(long),0,0) < 0)
	{
		perror("msgrcv failed\n");
		exit(EXIT_FAILURE);

	}


	printf("one message received\n");
	printf("msg.tpye is %d\n",msq.type);
	printf("msg.temperature is %f\n",msq.temperature);
	printf("msg.moisture is %f\n",msq.moisture);


	return 0;
}
