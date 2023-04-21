#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>
#include<signal.h>
#include<pthread.h>

#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

int sockfd;

pthread_t thread_run_cmd;
pthread_t thread_tcp_server;

void* run_cmd(void* msg)
{
	while(1)
	{
		printf("hello rum cmd\n");
		sleep(1);
	}

}

void* tcp_server(void* msg)
{
	char *port = (char*)msg;
	sockfd = socket(AF_INET,SOCK_STREAM,0);

	if(sockfd < 0)
	{
		perror("socket failed\n");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(atoi(port));
	serveraddr.sin_addr.s_addr = INADDR_ANY;

	if(bind(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr)) < 0)
	{
		perror("bind failed\n");
		exit(EXIT_FAILURE);
	}



	if(listen(sockfd,10) < 0 )
	{
		perror("listen failed\n");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in clientaddr;
	socklen_t clientaddr_len;


	while(1)
	{

		int fd = accept(sockfd,(struct sockaddr*)&clientaddr,&clientaddr_len);
		time_t t = time(NULL);
		char*st = ctime(&t);

		if(write(fd,st,strlen(st)) != strlen(st))
		{
			perror("write failed\n");
			exit(EXIT_FAILURE);
		}
		close(fd);


	}

}



void sig_handler(int signo)
{

	if(signo == SIGINT)
	{
		printf("farewell\n");
		exit(EXIT_FAILURE);

	}

}

int main(int argc,char* argv[])
{
	if(argc < 2)
	{
		printf("usage: %s port\n",argv[0]);
		exit(EXIT_FAILURE);
	}



	if(signal(SIGINT,sig_handler) == SIG_ERR)
	{
		perror("signal failed\n");
		exit(EXIT_FAILURE);
	}
	pthread_create(&thread_run_cmd,NULL,run_cmd,NULL);
	pthread_create(&thread_tcp_server,NULL,tcp_server,(void*)argv[1]);


	pthread_join(thread_run_cmd,NULL);
	pthread_join(thread_tcp_server,NULL);

	printf("end\n");

	return 0;
}
