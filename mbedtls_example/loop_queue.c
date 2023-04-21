#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>

typedef struct
{

	uint8_t* const buffer;
	int head;
	int tail;
	const int maxLen;
}circBuf_t;

#define BLE_CircularBuffSize 10//1000

uint8_t dataBuff[BLE_CircularBuffSize] = {0};
circBuf_t circularBuffer = {&dataBuff[0],0,0,BLE_CircularBuffSize};

int circBufPush(circBuf_t *c, uint8_t data)
{
	int next = c->head + 1;
	if(next >= c->maxLen)
		next =0;

	//Circular buffer is full
	if(next == c->tail)
		return -1;

	c->buffer[c->head] = data;
	c->head = next;

	return 0;
}

int circBufPop(circBuf_t *c, uint8_t *data)
{
	//if the head isn't ahead of the tail,we dont have any characters
	if(c->head == c->tail)
		return -1;

	*data = c->buffer[c->tail];
	//c->buffer[c->tail] = 0;//clear the data (optional)

	int next = c->tail +1;
	if(next >= c->maxLen)
		next =0;

	c->tail = next;

	return 0;
}


int circBuf_read_all(circBuf_t *c, uint8_t *data)
{
	int i=0;
	while(circBufPop(c,data+i) != -1)//reach tail
	{
		if(i >= BLE_CircularBuffSize)
		{
			return -1;
		}

	}

	return i;
}



int main(int argc,char* argv[])
{
	uint8_t testd[BLE_CircularBuffSize] = 
                {0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39};

	uint8_t teste[BLE_CircularBuffSize] = 
                {0x60,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69};
	circBuf_t* ptr_circBuf = &circularBuffer; 

	for(int i=0; i< BLE_CircularBuffSize;i++)
	{
		if(circBufPush(ptr_circBuf,testd[i]) == -1)
		{
			printf("circular buffer is full. and i is %d\n",i);

		}
	}



	for(int i=0; i< BLE_CircularBuffSize;i++)
	{
//		printf("dataBuff[%d] = %02x\n",i,dataBuff[i]);
	}

	int cnt=0;
	while(1)
	{
		uint8_t datav=0;
		int rv =  circBufPop(ptr_circBuf, &datav);
		if(rv == -1)
		{
			printf("queue is empty and quit\n");
			break;
		}
		else
		{
			printf("datav[%d] = %02x\n",cnt++,datav);

		}
	}

	return 0;

}
