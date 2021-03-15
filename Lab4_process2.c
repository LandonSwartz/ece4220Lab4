//process 2

#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<wiringPi.h>
#include<pthread.h>
#include<semaphore.h>
#include<unistd.h>
#include<sys/mman.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/time.h>

#define MY_PRIORITY 51

struct timeval time_of_btn; //when GPS happening

int main()
{
//scheduler
	struct sched_param param;
	param.sched_priority = MY_PRIORITY + 1; //to be most important priority 
	int ret = sched_setscheduler(0, SCHED_FIFO, &param);
	if(ret == -1)
	{
		printf("Error with scheduler\n");
	}
	
	//button
	int fd = open("/dev/mem", O_RDWR | O_SYNC);
	if(fd < 0)
	{
		printf("can't open dev/mem\n");
		exit(1);
	}
	
	//pipeline
	char * namedPipe2 = "/tmp/N_pipe2";
	if(mkfifo(namedPipe2, 0777) == 0)
	{
		printf("N-pipe-2 created\n");
	}
	else
	{
		printf("N-pipe-2 has already been created...\n");
	}

	int fd2;
	fd2 = open(namedPipe2, O_RDWR);
	int bytes;
	
	unsigned long * gpeds = mmap(NULL, getpagesize(), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x3F200000);
	gpeds = gpeds + (0x00000040/4); //setting offset
	
	while(1)
	{
		if(*gpeds == 0x00010000)
		{	
			gettimeofday(&time_of_btn, NULL);

			printf("Button press happening\n");
			
			bytes = write(fd2, &time_of_btn, sizeof(struct timeval));
			
			//clear the reg like lab three
			*gpeds = 0xFFFFFFFF;

		}
		
		printf("Waiting\n");
		//waiting until next period
		delay(60); //60 ms
	}

}
