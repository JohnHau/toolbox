#include <stdio.h>      
#include <stdlib.h>    
#include <string.h>       
#include <unistd.h>      
#include <sys/types.h>   
#include <sys/stat.h>  
#include <fcntl.h>      
#include <termios.h>   
#include <errno.h>       
#include <getopt.h>   
#include <time.h>     
#include <sys/select.h>    
#include "com_uart_port.h"

int setOpt(int fd, int nSpeed, int nBits, int nParity, int nStop)
{
    struct termios newtio, oldtio;

    // get current parameters
    if (tcgetattr(fd, &oldtio) != 0)
    {
        perror("SetupSerial 1");
        return -1;
    }

    //clear new termios parameters
    bzero(&newtio, sizeof(newtio));   


    //CLOCAL -- ignore modem control line and local line
    //CREAD -- enable receiving
    newtio.c_cflag |= CLOCAL | CREAD; 


    // set data bits
    // clear data bits
    newtio.c_cflag &= ~CSIZE;   

    switch (nBits)
    {
        case 7:
            newtio.c_cflag |= CS7;
        break;
        case 8:
            newtio.c_cflag |= CS8;
        break;
        default:
            fprintf(stderr, "Unsupported data size\n");
            return -1;
    }

    switch (nParity)
    {
        case 'o':
        case 'O'://odd
            newtio.c_cflag |= PARENB;
            newtio.c_cflag |= PARODD;
            newtio.c_iflag |= (INPCK | ISTRIP);
            break;
        case 'e':
        case 'E'://even
            newtio.c_iflag |= (INPCK | ISTRIP);
            newtio.c_cflag |= PARENB;
            newtio.c_cflag &= ~PARODD;
            break;
        case 'n':
        case 'N':
            newtio.c_cflag &= ~PARENB;
            break;
        default:
            fprintf(stderr, "Unsupported parity\n");
            return -1;
    }

    switch (nStop)
    {
        case 1:
            newtio.c_cflag &= ~CSTOPB;
        break;
        case 2:
            newtio.c_cflag |= CSTOPB;
        break;
        default:
            fprintf(stderr,"Unsupported stop bits\n");
            return -1;
    }


    //set baudrate 2400/4800/9600/19200/38400/57600/115200/230400
    switch (nSpeed)
    {
        case 2400:
            cfsetispeed(&newtio, B2400);
            cfsetospeed(&newtio, B2400);
            break;
        case 4800:
            cfsetispeed(&newtio, B4800);
            cfsetospeed(&newtio, B4800);
            break;
        case 9600:
            cfsetispeed(&newtio, B9600);
            cfsetospeed(&newtio, B9600);
            break;
        case 19200:
            cfsetispeed(&newtio, B19200);
            cfsetospeed(&newtio, B19200);
            break;
        case 38400:
            cfsetispeed(&newtio, B38400);
            cfsetospeed(&newtio, B38400);
            break;
        case 57600:
            cfsetispeed(&newtio, B57600);
            cfsetospeed(&newtio, B57600);
            break;
        case 115200:
            cfsetispeed(&newtio, B115200);
            cfsetospeed(&newtio, B115200);
            break;
        case 230400:
            cfsetispeed(&newtio, B230400);
            cfsetospeed(&newtio, B230400);
            break;
        default:
            printf("\tSorry, Unsupported baud rate, set default 9600!\n\n");
            cfsetispeed(&newtio, B9600);
            cfsetospeed(&newtio, B9600);
            break;
    }



    // 1 character, wait 1*(1/10)s
    newtio.c_cc[VTIME] = 1;


    // the least character read
    newtio.c_cc[VMIN] = 1; 




    tcflush(fd,TCIFLUSH); //clear buffer

    //enable new settings
    if (tcsetattr(fd, TCSANOW, &newtio) != 0) 
    {
	    perror("SetupSerial 3");
	    return -1;
    }

    printf("Serial set done!\n");

    return 0;

}



int32_t init_com_port(char* port,uint32_t baudrate)
{
	int fdSerial;

	fdSerial = open(port, O_RDWR | O_NOCTTY | O_NDELAY);

	if(fdSerial < 0)
	{
		perror(port);
		return -1;
	}

	//uart 0=blocked  FNDELAY=non-blocked
	//blocked, even open() with non-blocked
	if (fcntl(fdSerial, F_SETFL, 0) < 0)   						
	{
		printf("fcntl failed!\n");
	}
	else
	{
		printf("fcntl=%d\n", fcntl(fdSerial, F_SETFL, 0));
	}


	if (isatty(fdSerial) == 0)
	{
		printf("standard input is not a terminal device\n");
		close(fdSerial);
		return -1;
	}
	else
	{
		printf("is a tty success!\n");
	}
	printf("fd-open=%d\n", fdSerial);


	if (setOpt(fdSerial, baudrate, 8, 'N', 1)== -1) 
	{
		fprintf(stderr, "Set opt Error\n");
		close(fdSerial);
		exit(1);
	}
	//clear uart buffer
	tcflush(fdSerial, TCIOFLUSH); 

	//uart blocked
	fcntl(fdSerial, F_SETFL, 0);


	return fdSerial;

}

