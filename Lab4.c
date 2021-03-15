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
#include<sys/time.h>

sem_t sema; //semaphore for whatever
//struct timeval stamp_time;
struct timeval GPS; //struct for time of gps

/*struct GPS_DATA
{
	struct timeval X1;
	struct timeval X2;
	struct timeval X_before_btn_press;
	int Y1;
	int Y2;
	int slope;
	int Y_before_btn_press; 
};*/

char buffer[2]; //buffer for reading from fd

//thread for receiving real time event
//Will dynamically make child threads for button presses
int timeval_subtract(struct timeval *result, struct timeval *x, struct timeval *y)
{
  struct timeval xx = *x;
  struct timeval yy = *y;
  x = &xx; y = &yy;

  if (x->tv_usec > 999999)
  {
    x->tv_sec += x->tv_usec / 1000000;
    x->tv_usec %= 1000000;
  }

  if (y->tv_usec > 999999)
  {
    y->tv_sec += y->tv_usec / 1000000;
    y->tv_usec %= 1000000;
  }

  result->tv_sec = x->tv_sec - y->tv_sec;

  if ((result->tv_usec = x->tv_usec - y->tv_usec) < 0)
  {
    result->tv_usec += 1000000;
    result->tv_sec--; // borrow
  }

  return result->tv_sec < 0;
}

void *Pthread0()
{
	int fd;
	char * namedPipe2 = "/tmp/N_pipe2";
	fd = open(namedPipe2, O_RDONLY); //reading second named pipe to recieve real time event
	
	//struct GPS_DATA data;
	int bytes2; //number of bytes read in from pipeline 2
	
	struct timeval X_before_btn_press;
	struct timeval X1;
	struct timeval X2;
	int Y1;
	int Y2;
	int slope; 
	int Y_before_btn_press;
	
	close(fd);
	
	while(1)
	{
		bytes2 = read(fd, &X_before_btn_press, sizeof(int));
		
		Y1 = buffer[0];
		X1 = GPS; //<-^ from main thread 
		
		while(Y1 == buffer[0])
		{
			//waiting for buffer to update next data point before moving on
		} 
		
		Y2 = buffer[0];
		X2 = GPS; //<-^ from main thread 
		
		//finding slope
		int x_result = timeval_subtract(&x_result, &X1, &X2);
		
		slope = (Y2 - Y1) / x_result;
		
		int x_result2 = timeval_subtract(&x_result2, &X_before_btn_press, &X1);
		
		//finding Y_before_btn_press
		Y_before_btn_press = ((slope * x_result2) + Y1);
		
		printf("y1 is: %d\n", Y1);
		printf("y2 is: %d\n", Y2);
		printf("x1 is: %d\n", X1);
		printf("x2 is: %d\n", X2);
		printf("X coordinate of button press is %d", X_before_btn_press);
		printf("Y coordinate of button press is %d", Y_before_btn_press);
	}

}




//thread for interpolating the data from Pthread0
/*void * childThread(void *args)
{

}*/


int main()
{
	char * namedPipe1 = "/tmp/N_pipe1";
	char * namedPipe2 = "/tmp/N_pipe2";

	
	//struct timeval GPS; //struct for time of gps
	int bytes; //temp value for reading in
	
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
	
	pthread_create(&thread0_EventThread, NULL, Pthread0, NULL);


	//setting GPS struct
	gettimeofday(&GPS, NULL);
	//getting gps signal
	while(1)
	{
		int fd = open(namedPipe1, O_RDWR);
		
		if(read(fd, &buffer, sizeof(unsigned char)) < 0) //error because recieving zeros I believe
		{
			printf("Can't read n_pipe1...\n");
		}
		//reading from pipeline
		
		bytes = read(fd, &buffer, sizeof(char)); //reading into buffer and setting to str1
		gettimeofday(&GPS, NULL);
		printf("Number of bytes read: %d\n", bytes);
		if(bytes == -1)
			printf("No GPS signal being recieved\n");
		else
			printf("GPS signal: %c\n", buffer[0]);

		//printf("Time of day is: %ld.%06ld\n", GPS.tv_sec, GPS.tv_usec); //from stackoverflow for writing nicely
		close(fd);
		delay(250); //waiting period of 250 ms	
	}	
	
	pthread_join(thread0_EventThread, NULL);
	
	pthread_exit(NULL);
}





