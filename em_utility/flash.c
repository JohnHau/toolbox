#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<pthread.h>
#include<mqueue.h>
#define FLASH_SECTOR_SIZE (4096U)
#define FLASH_TOTAL_SECTOR  (4U)
#define FLASH_SIZE (FLASH_SECTOR_SIZE * FLASH_TOTAL_SECTOR) 
 

char *flash_info_file_name =  "./flash_info.log";
char *flash_file_name      =  "./flash.log";


typedef struct emflash_info
{
	int fd_flash_info;
	int fd_flash_file;
	uint32_t flash_size;
	uint32_t flash_sector_size;
	uint32_t flash_total_sector;

	char flash_info_file[32];
	char flash_file[32];

}emflash_info_t;

emflash_info_t g_flash_info;

int32_t flash_erase_whole_chip(emflash_info_t *flash_info)
{
	int fd;
	int i;
	int j;

	char c = 0xff; 
#if 0


	for(i=0; i< flash_info->flash_total_sector;i++)
	{
		fd = open(flash_info->flash_sector_file[i],O_RDONLY|O_CREAT| O_TRUNC,0666);
		if(fd < 0)
		{
			printf("open %s failed\n",flash_info->flash_sector_file[i]);
			return -1;
		}

		for(j=0; j< flash_info->flash_sector_size;j++)
		{
			write(fd,&c,1);
		}		

		close(fd);

	}



#endif



}



int32_t flash_open(emflash_info_t *flash_info)
{
	int i = 0;
	int j = 0;
	int fd;
	int n;
	char c = 0xFF;

	flash_info-> flash_sector_size  = FLASH_SECTOR_SIZE;
	flash_info-> flash_total_sector = FLASH_TOTAL_SECTOR;
	flash_info-> flash_size         = FLASH_SIZE;

	memset(flash_info-> flash_info_file,0,sizeof(flash_info_file_name));
	memset(flash_info-> flash_file,0,sizeof(flash_file_name));

	strncpy(flash_info-> flash_info_file,flash_info_file_name,strlen(flash_info_file_name));
	strncpy(flash_info-> flash_file,flash_file_name,strlen(flash_file_name));

	fd = open(flash_info->flash_info_file,O_RDWR|O_CREAT,0666);

	if(fd < 0)
	{
		printf("open file %s failed\n",flash_info->flash_info_file);
		return -1;
	}

	n = read(fd,flash_info,sizeof(emflash_info_t));

	if(n == 0)
	{
		if(write(fd,flash_info,sizeof(emflash_info_t)) != sizeof(emflash_info_t))
		{
			printf("write file %s failed\n",flash_info->flash_info_file);
			return -1;

		}

		close(fd);


		printf("opening %s\n",flash_info->flash_file);

		fd = open(flash_info->flash_file,O_WRONLY|O_CREAT| O_TRUNC,0666);
		if(fd < 0)
		{
			printf("open %s failed\n",flash_info->flash_file[i]);
			return -1;	
		}

		printf("total sector is %d\n",flash_info->flash_total_sector);

		for(i=0;i< flash_info->flash_total_sector;i++)//fill 0xFF each sector for whole chip
		{
			printf("sector size is %d\n",flash_info->flash_sector_size);
			for(j=0; j< flash_info->flash_sector_size;j++)
			{
				write(fd,&c,1);
			}		
		}


		flash_info->fd_flash_file = fd;

	}
	else if(n == sizeof(emflash_info_t))
	{
		printf("flash_info-> flash_size is %d\n",flash_info-> flash_size);
		printf("flash_info-> flash_total_sector is %d\n",flash_info-> flash_total_sector);
		printf("flash_info-> flash_sector_size is %d\n",flash_info-> flash_sector_size);

		printf("flash_info-> flash_file is %s\n",flash_info-> flash_file);

		if(flash_info-> flash_sector_size  == FLASH_SECTOR_SIZE  &&
				flash_info-> flash_total_sector == FLASH_TOTAL_SECTOR &&
				flash_info-> flash_size         == FLASH_SIZE )
		{
			printf("flash has created\n");

		}
		else
		{

			printf("flash created faultly\n");
			return -1;
		}


		printf("now openning %s\n",flash_info->flash_file);

		fd = open(flash_info->flash_file,O_RDWR,0666);

		if(fd < 0)
		{
			printf("open %s failed\n",flash_info->flash_file);
			return -1;
		}

		flash_info->fd_flash_file = fd;
	}

	return 0;
}


int32_t	flash_close(emflash_info_t *flash_info)
{

	close(flash_info->fd_flash_file);
	return 0;
}



int32_t flash_read_one_sector(emflash_info_t *flash_info,uint32_t n_sector,char*buf,uint32_t buf_size)
{
	if(n_sector >= flash_info->flash_total_sector)//out of sector range	
	{
		return -1;
	}

	if(buf_size < flash_info->flash_sector_size)//out of sector size	
	{
		return -1;
	}

	lseek(flash_info->fd_flash_file,0,SEEK_SET);
	lseek(flash_info->fd_flash_file,n_sector*flash_info->flash_sector_size,SEEK_SET);

	if(read(flash_info->fd_flash_file,buf,flash_info->flash_sector_size) != flash_info->flash_sector_size)
	{
		printf("flash_read_one_sector failed\n");
		return -1;
	}

	return 0;
}

int32_t flash_write_one_sector(emflash_info_t *flash_info,uint32_t n_sector,char*buf,uint32_t buf_size)
{
	if(n_sector >= flash_info->flash_total_sector)//out of sector range	
	{
		return -1;
	}

	if(buf_size < flash_info->flash_sector_size)//out of sector size	
	{
		return -1;
	}

	lseek(flash_info->fd_flash_file,0,SEEK_SET);
	lseek(flash_info->fd_flash_file,n_sector*flash_info->flash_sector_size,SEEK_SET);

	if(write(flash_info->fd_flash_file,buf,flash_info->flash_sector_size) != flash_info->flash_sector_size)
	{
		printf("flash_read_one_sector failed\n");
		return -1;
	}

	return 0;
}




int32_t flash_read_sector(emflash_info_t *flash_info,uint32_t n_sector,uint32_t offset,char*buf,uint32_t buf_size)
{
	if(n_sector >= flash_info->flash_total_sector)//out of sector range	
	{
		printf("flash_read_sector: out of sector range\n");
		return -1;
	}


	if(offset > flash_info->flash_sector_size)
	{
		printf("flash_read_sector: offset out of sector range\n");
		return -1;
	}

	if(buf_size > (flash_info->flash_sector_size - offset))//make sure reading proper data size
	{
		printf("flash_read_sector: buf_size out of sector range\n");
		return -1;
	}

	lseek(flash_info->fd_flash_file,0,SEEK_SET);
	int n = lseek(flash_info->fd_flash_file,n_sector*flash_info->flash_sector_size + offset,SEEK_SET);
	printf("offset n is %d\n",n);

	if(read(flash_info->fd_flash_file,buf,buf_size) != buf_size)
	{
		printf("flash_read_sector failed\n");
		return -1;
	}

	return 0;
}


int32_t flash_write_sector(emflash_info_t *flash_info,uint32_t n_sector,uint32_t offset,char*buf,uint32_t buf_size)
{
	if(n_sector >= flash_info->flash_total_sector)//out of sector range	
	{
		printf("flash_write_sector: out of sector range\n");
		return -1;
	}


	if(offset > flash_info->flash_sector_size)
	{
		printf("flash_write_sector: offset out of sector range\n");
		return -1;
	}

	if(buf_size > (flash_info->flash_sector_size - offset))//make sure reading proper data size
	{
		printf("flash_write_sector: buf_size out of sector range\n");
		return -1;
	}

	lseek(flash_info->fd_flash_file,0,SEEK_SET);
	int n = lseek(flash_info->fd_flash_file,n_sector*flash_info->flash_sector_size + offset,SEEK_SET);
	printf("offset n is %d\n",n);

	if(write(flash_info->fd_flash_file,buf,buf_size) != buf_size)
	{
		printf("flash_write_sector failed\n");
		return -1;
	}

	return 0;
}





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





char out_buffer[4096] = {0};
char in_buffer[4096] = {0};

int32_t fill_buffer_with_random_number(char*buf,uint32_t buf_size)
{
	int i = 0;
	time_t seed = time(NULL);
	srand(seed);

	for(i=0;i<buf_size;i++)
	{
		buf[i] = rand()%0xFF;
	}
	return 0;
}


int main(int argc,char* argv[])
{
	fill_buffer_with_random_number(out_buffer,sizeof(out_buffer));
	flash_open(&g_flash_info);

#if 1
	//flash_write_one_sector(&g_flash_info,3,out_buffer,4096);

	//flash_read_one_sector(&g_flash_info,3,in_buffer,4096);


	flash_write_sector(&g_flash_info,2,8,out_buffer,8);
	flash_read_sector(&g_flash_info,2,8,in_buffer,8);
	int flag =0;
	//for(int i=0;i<4096;i++)
	for(int i=0;i<8;i++)
	{
		if(out_buffer[i] != in_buffer[i])
		{
			flag =1;
			break;
		}

	}

	if(flag ==1)
	{
		printf("not same\n");
	}
	else
	{
		printf("the same\n");
	}

#endif


	flash_close(&g_flash_info);



	/*
	   char str[3] = {0};
	   for(int i=0; i<256; i++)
	   {
	   byte2hexstring(i,str);
	   }

*/

	return 0;

}
