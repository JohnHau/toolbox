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

	if((msqid = msgget(mkey,IPC_CREAT|IPC_EXCL|0777)) < 0 )
	{
		perror("msgget failed\n");
		exit(EXIT_FAILURE);
	}


	msq.type = 1;
	msq.temperature = 35.0;
	msq.moisture = 35.0;

	if(msgsnd(msqid,&msq,sizeof(msq)-sizeof(long),0) < 0)
	{
		perror("msgsnd failed\n");
		exit(EXIT_FAILURE);

	}


	printf("one message sent\n");


	return 0;
}
