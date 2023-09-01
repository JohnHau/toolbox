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
//
//FF FF FF FF 'F' 'H' Ve Ad T L V... CS 
//                          fixed
//Tag
//0x31:t
//0x32:h
//char msg1[] = {'F','H',0x01,0x01,}
#define FH_PTCL_HDR_H  'F'
#define FH_PTCL_HDR_L  'H'

#define FH_PTCL_TAG_T   0x01 
#define FH_PTCL_TAG_H   0x02 
#define FH_PTCL_TAG_A   0x80 

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


uint8_t cal_checksum_ptcl_msg(uint8_t*msg,uint32_t len)
{
	int32_t i = 0;
	uint8_t ck =0;

	for(i=0;i< len;i++)
	{
		ck += msg[i];
	}

	return ck;

}









typedef enum ptcl_msg_recv_state
{
	state_pream = 1,
	state_hdr,
	state_payload_len,
	state_payload,
	state_crc
}ptcl_msg_recv_state_t;

uint8_t g_msg_buffer[64] = {0};
uint8_t g_msg_send_buffer[64] = {0};

int32_t receive_ptcl_msg(int fd,uint8_t*msg)
{

	uint8_t cnt_pream = 0;
	uint8_t cnt = 0;
	uint8_t vlen = 0;

	uint8_t ch = 0;
	uint8_t n = 0;
	int retval = 0;

	struct timeval tv;
	tv.tv_sec =0;
	tv.tv_usec = 50000;//50ms

	fd_set rfds;
	FD_ZERO(&rfds);
	FD_SET(fd,&rfds);

	ptcl_msg_recv_state_t state = state_pream ;

	while(1)
	{
		retval = select(fd+1,&rfds,NULL,NULL,&tv);
		if(retval == -1)
		{
			perror("select\n");
		}
		else if(retval)
		{
			n = read(fd,&ch,1);
			if(n == 1)
			{
				switch(state)
				{
					case state_pream: 

						if(ch == 0xFF)
						{
							cnt_pream ++;
						}

						if(cnt_pream == 4)
						{
							state = state_hdr;
						}
						break;

					case state_hdr: 
						msg[cnt++] = ch; 
						if(cnt == 5)
						{
							state = state_payload_len;
						}

						break;

					case state_payload_len: 
						vlen = ch;
						msg[cnt++] = vlen; 
						state = state_payload;
						break;

					case state_payload: 
						for(int i=0; i< vlen;i++)
						{
							msg[cnt++] = ch; 
						}

						state = state_crc;

						break;
					case state_crc: 
						msg[cnt] = ch; 
						break;

					default:
						break;

				}
			}
			else//broken payload and abandomn
			{
				state = state_pream;
				cnt = 0;
				vlen = 0;
			}

		}
		else//select time out and abandomn
		{
			state = state_pream;
			cnt = 0;
			vlen = 0;
		}

	}

}


int32_t send_ptcl_msg(int fd,m_dat_t* s_dat,uint8_t s_tag)
{

	switch(s_tag)
	{
		case  FH_PTCL_TAG_T: 
			g_msg_send_buffer[FH_MSG_TAG] = 0x80|FH_PTCL_TAG_T;
			g_msg_send_buffer[6] = s_dat->ta[0];
			g_msg_send_buffer[7] = s_dat->ta[1];
			g_msg_send_buffer[8] = s_dat->ta[2];
			g_msg_send_buffer[9] = s_dat->ta[3];
			break;

		case  FH_PTCL_TAG_H: 
			g_msg_send_buffer[FH_MSG_TAG] = 0x80|FH_PTCL_TAG_H;
			g_msg_send_buffer[6] = s_dat->ha[0];
			g_msg_send_buffer[7] = s_dat->ha[1];
			g_msg_send_buffer[8] = s_dat->ha[2];
			g_msg_send_buffer[9] = s_dat->ha[3];

			break;
		default:
			return -1;
			break;

	}

	g_msg_send_buffer[0] = FH_PTCL_HDR_H;
	g_msg_send_buffer[1] = FH_PTCL_HDR_L;
	g_msg_send_buffer[FH_MSG_VER] = 0x01;
	g_msg_send_buffer[FH_MSG_ADDR] = MY_ADDR;
	//g_msg_send_buffer[4];//TAG
	g_msg_send_buffer[FH_MSG_LEN] = 0x04;
	//g_msg_send_buffer[6];
	//g_msg_send_buffer[7];
	//g_msg_send_buffer[8];
	//g_msg_send_buffer[9];
	//g_msg_send_buffer[10];

	g_msg_send_buffer[10]= cal_checksum_ptcl_msg(g_msg_send_buffer,10);
	write(fd,g_msg_send_buffer,11);

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





void* recv_msg(void*arg)
{
	int fd; 
	char *myfifo = "/tmp/myfifo";
	uint8_t mbuf[64] = {0};

	mkfifo(myfifo,0666);
	fd = open(myfifo,O_RDONLY);

	if(fd < 0)
	{
		perror("open myfifo");
		exit(EXIT_FAILURE);
	}

	while(1)
	{
		uint8_t n = 0;
start:
		n = read(fd,mbuf,64);

		if(n <= 0)
		{
			close(fd);
			fd = open(myfifo,O_RDONLY);

			if(fd < 0)
			{
				perror("open myfifo");
				exit(EXIT_FAILURE);
			}

			goto start;
		}
		else if(n > 0)
		{

			printf("msg len is %d\n",n);
			printf("msg is %s\n",mbuf);
		}


#if 0
		receive_ptcl_msg(fd,g_msg_buffer);
		printf("thread recv_msg\n");
		sleep(2);
#endif
	}

}




void* parse_msg(void*arg)
{

	while(1)
	{
		parse_ptcl_msg(g_msg_buffer,0x01);
		printf("thread parse_msg\n");
		sleep(6);

	}

}








int main(int argc, char* argv[])
{

	pthread_t th_recv_msg;
	pthread_t th_parse_msg;

	pthread_create(&th_recv_msg,NULL,recv_msg,NULL);
	pthread_create(&th_parse_msg,NULL,parse_msg,NULL);
	pthread_join(th_recv_msg,NULL);
	pthread_join(th_parse_msg,NULL);



	gen_msg(test_msg,FH_PTCL_TAG_T,9.8);
	parse_ptcl_msg(test_msg,0x01);
	printf("cur_data.t is %2.1f\n",cur_data.t);



	gen_msg(test_msg,FH_PTCL_TAG_H,81.2);
	parse_ptcl_msg(test_msg,0x01);
	printf("cur_data.h is %2.1f\n",cur_data.h);

	return 0;
}


