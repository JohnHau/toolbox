#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>
#include<pthread.h>

#include<sys/select.h>

#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

int sockfd;
struct ip_port
{
	char ip[32];
	char port[32];
};

pthread_t thread_client_tcp;

void* client_tcp(void*msg)
{
	struct ip_port   * ptr_ip_and_port = msg;
	char*ip = ptr_ip_and_port->ip;
	char*port = ptr_ip_and_port->port;

	sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(sockfd < 0)
	{
		perror("socket failed\n");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(atoi(port));
	inet_pton(AF_INET,ip,&serveraddr.sin_addr.s_addr);

	if(connect(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr)) < 0)
	{
		perror("connect failed\n");
		exit(EXIT_FAILURE);
	}

	char cbuf[32] = {0};

	fd_set rfds;
	int retval;

	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 5000;
	
	while(1)
	{

		FD_ZERO(&rfds);
		FD_SET(sockfd,&rfds);

		retval = select(sockfd + 1,&rfds,NULL,NULL,NULL);

		memset(cbuf,0,sizeof(cbuf));
		if(retval == -1)
		{
			perror("select error");
		}
		else if(retval)
		{
			if(FD_ISSET(sockfd,&rfds))
			{

				size_t rsize = read(sockfd,cbuf,30);

				if(rsize == 0)
				{
					printf("server fd closed\n");
					exit(EXIT_FAILURE);
				}

				printf("message is %s\n",cbuf);
			}

		}
		else
		{
			printf("select nnnnnn\n");
		}

	}


}









void sig_handler(int signo)
{
	if(signo == SIGINT)
	{
		printf("farewell client\n");
		close(sockfd);
		exit(EXIT_FAILURE);
	}

}

int main(int argc,char* argv[])
{
	if(argc < 3)
	{
		printf("usage: %s ip port\n",argv[0]);
	}

	if(signal(SIGINT,sig_handler) == SIG_ERR)
	{
		perror("signal failed\n");
		exit(EXIT_FAILURE);
	}


	struct ip_port ip_and_port;
	strcpy(ip_and_port.ip,argv[1]);
	strcpy(ip_and_port.port,argv[2]);

	pthread_create(&thread_client_tcp,NULL,client_tcp,(void*)&ip_and_port);
	pthread_join(thread_client_tcp,NULL);

	printf("end\n");

	return 0;

}
