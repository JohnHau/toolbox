#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/time.h>
#include<fcntl.h>
#include<unistd.h>
#include<pthread.h>
#include<mqueue.h>
#include<semaphore.h>
/*
 *convert one byet to one hexadecimal string 
 * */

uint8_t byte2hexstring(uint8_t val,char* array)
{

	array[0] = val/16;
	array[1] = val%16;
	array[2] = '\0';


	if(array[0] >= 0 && array[0] <=9)
	{
		array[0] += '0';
	}
	else if(array[0] >= 10 && array[0] <=15)
	{

		array[0] -= 10;
		array[0] += 'A';
	}



	if(array[1] >= 0 && array[1] <=9)
	{
		array[1] += '0';
	}
	else if(array[1] >= 10 && array[1] <=15)
	{

		array[1] -= 10;
		array[1] += 'A';
	}

	return 0;

}


//FF FF FF FF 'F' 'H' Ve Ad T L V... CS 
//                          fixed
//Tag
//0x31:t
//0x32:h
//char msg1[] = {'F','H',0x01,0x01,}
#define FH_PTCL_HDR_H  'F'
#define FH_PTCL_HDR_L  'H'
 
#define FH_PTCL_TAG_T   0x31 
#define FH_PTCL_TAG_H   0x32 

#define FH_MSG_VER      0x02 
#define FH_MSG_ADDR     0x03 
#define FH_MSG_TAG      0x04 
#define FH_MSG_LEN      0x05 

#define MY_ADDR  0x01
typedef struct m_dat
{
	union
	{
		float t;
		uint8_t ta[4];
	};

	union
	{
		float h;
		uint8_t ha[4];
	};

	uint8_t addr;
}m_dat_t;

m_dat_t cur_data = {0};

int32_t checksum_ptcl_msg(uint8_t*msg)
{
	int32_t len = msg[FH_MSG_LEN];
	int32_t i = 0;
	uint8_t ck =0;

	len += 6;//plus 6 is the total length of msg
	printf("len ptcl is %d\n",len);
	for(i=0;i< len;i++)
	{
		ck += msg[i];
	}

	if(ck != msg[len])
	{
		return -1;
	}

	return 0;

}


int32_t parse_ptcl_msg(uint8_t*msg,uint8_t ver)
{
	int32_t i =0;
	uint8_t* p = NULL;

	if(msg[0] != FH_PTCL_HDR_H || msg[1] != FH_PTCL_HDR_L)	
	{
		return -1;
	}

	if(msg[2] = 0x01)//check ver = 0x01
	{
		if(msg[3] == MY_ADDR)//check if msg for us
		{
			if(checksum_ptcl_msg(msg) < 0)
			{
				printf("invalid message\n");
				return -1;
			}

			switch(msg[4])//check Tag	
			{
				case FH_PTCL_TAG_T:
					cur_data.ta[0] = msg[6]; 
					cur_data.ta[1] = msg[7]; 
					cur_data.ta[2] = msg[8]; 
					cur_data.ta[3] = msg[9]; 

					break;

				case FH_PTCL_TAG_H:
					cur_data.ha[0] = msg[6]; 
					cur_data.ha[1] = msg[7]; 
					cur_data.ha[2] = msg[8]; 
					cur_data.ha[3] = msg[9]; 

					break;

				default:
					break;

			}

		}

	}


}



union fiv
{
	float x;
	uint8_t xa[4];
};

uint8_t test_msg[16]={'F','H',0x01,0x01,0x31,0x04};

void gen_msg(uint8_t* msg,uint8_t t,float v)
{
	union fiv fv;
	uint8_t ck =0;
	int i =0;

	fv.x = v;

	msg[4] = t;

	msg[6] = fv.xa[0];
	msg[7] = fv.xa[1];
	msg[8] = fv.xa[2];
	msg[9] = fv.xa[3];

	for(i=0;i<10;i++)
	{
		ck += msg[i];
	}

	msg[i] = ck;

}

int main(int argc, char* argv[])
{

	gen_msg(test_msg,FH_PTCL_TAG_T,9.8);
	parse_ptcl_msg(test_msg,0x01);
	printf("cur_data.t is %2.1f\n",cur_data.t);



	gen_msg(test_msg,FH_PTCL_TAG_H,81.2);
	parse_ptcl_msg(test_msg,0x01);
	printf("cur_data.h is %2.1f\n",cur_data.h);

	return 0;
}


