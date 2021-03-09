//Lab4 RT Embedded Computing
//Landon Swartz
//Inter-process communication (IPC) and synchronization, Dynamic Threads, and RT tasks

/* Run ./GPS in one terminal
Second process in second terminal: insmod Kernel Module (change from rising edge to high state)
GPHEN0 register changed
then ./Button_task
if(btn_pressed) //check every 60ms 
{
	//grab timestampe
	//send through pipe
}
//wait for next period after btn pressed, 60ms

third terminal is process 1, check every 250 ms
./Comm_Process
Find exact location of snapchat by finding the slope between two GPS data value,
	one before and after event of btn pressed happens
	use y2-y0/x2-x0 = y2-y1/x2-x1
int main() {
	//create communicationThread
	while(1){
		//read n-pipe1 (from GPS), blocking function
		//generate the timestamp, make global
		
	}
	
	communicationThread(){
		//open pipe first
		while(1):
		//read n-pipe2 (from process 2)
		//TS button pressed; Location of button pressed?
		Lgp = Lg
		TSgp = TSg
		//wait for next GPS
	}

}*/

//helpful links
//https://www.geeksforgeeks.org/named-pipe-fifo-example-c-program/

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

sem_t sema; //semaphore for whatever

void *EventThread();

int main()
{
	char * namedPipe1 = "/tmp/N_pipe1";
	char * namedPipe2 = "/tmp/N_pipe2";
	
	pthread_t thread0_EventThread;
	
	if(mkfifo(namedPipe1, 0777) == 0)
	{
		printf("N_pipe1 created\n");
	}
	else
	{
		printf("N_pipe1 alreayd created...\n");
	}
	
	if(mkfifo(namedPipe2, 0777) == 0)
	{
		printf("N-pipe-2 created\n");
	}
	else
	{
		printf("N-pipe-2 has already been created...\n");
	}
	
	int pfd = open(namedPipe1, O_RDWR);
	
	pthread_create(&thread0_EventThread, NULL, EventThread, NULL);
	pthread_join(thread0_EventThread, NULL);

	char str1[80];
	
	while(1)
	{
		int fd = open(namedPipe1, O_RDWR);

		read(fd, str1, 20);
		printf("GPS reads: %s\n", str1);
		close(fd);
		
		delay(250);
	}
	
	pthread_exit(NULL);
}

//thread for receiving real time event
void *EventThread()
{
	
}



