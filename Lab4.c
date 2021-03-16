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

/*Interpolation thread
{
	//wait until next GPS is available
	//interpolate
	sem_wait() for the pipe
	print to screen through simple pipe
	-- sem_post() is done in printing -- 
}
*/

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
struct timeval GPS; //struct for time of gps
char buffer[2]; //buffer for reading from fd

//struct of shared memory for interpolation threads, don't know if works
struct SharedMemory
{
	struct timeval X_before_btn_press;
	//X_before_btn_press.tv_sec = 0;
	//X_before_btn_press.tv_usec = 0;
	struct timeval X1;
	struct timeval X2;
	double Y1; //don't know why it works with doubles
	double Y2;
	double Y_before_btn_press;
};

//thread for receiving real time event
//Will dynamically make child threads for button presses
void *Pthread0()
{
	int fd;
	char * namedPipe2 = "/tmp/N_pipe2";
	fd = open(namedPipe2, O_RDONLY); //reading second named pipe to recieve real time event

	int bytes2; //number of bytes read in from pipeline 2
	
	struct timeval X_before_btn_press;
	X_before_btn_press.tv_sec = 0;
	X_before_btn_press.tv_usec = 0;
	struct timeval X1;
	struct timeval X2;
	double Y1; //don't know why it works with doubles
	double Y2;
	double Y_before_btn_press;
	

	
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
		
		//finding slopes and doing interpolation
		//finding the milliseconds of the slope
		int slope_milli = ((X2.tv_sec - X1.tv_sec) + ((int)X2.tv_usec - (int)X2.tv_usec)/1000000); 
		
		int slope2_milli = ((X_before_btn_press.tv_sec - X1.tv_sec) + ((int)X2.tv_usec - (int)X2.tv_usec)/1000000);
		
		//finding Y_before_btn_press
		Y_before_btn_press = (((Y2-Y1)*slope_milli) * (slope2_milli)) + Y1;
		
		//need to look into the printing/operation to fix not having decimals
		printf("y1 is: %lf\n", Y1);
		printf("y2 is: %lf\n", Y2);
		printf("x1 is: %f\n", (float)X1.tv_sec);
		printf("x2 is: %f\n", (float)X2.tv_sec);
		printf("X coordinate of button press is %f\n", (float)X_before_btn_press.tv_sec);
		printf("Y coordinate of button press is %lf\n", Y_before_btn_press);
	}
	close(fd);
}




//thread for interpolating the data from Pthread0
/*void * childThread(void *args)
{

}*/

//simple pipe and print thread
/*void Pthread1(void *args)
{
	//reading from pipe, use named pipe
	//print
	//sem_post the semaphore for printing
	
	int bytes;
	int fd[2];
	pipe(fd);
	
	struct timeval GPS;
	GPS.tv_sec = 0;
	GPS.tv_usec = 0;
	
	//reading in fd[0] to buffer of GPS
	if((bytes = read(fd[0], &GPS, sizeof(struct timeval)) >= 0)
	{
		//print the stuff 
	}
	
}*/


int main()
{
	char * namedPipe1 = "/tmp/N_pipe1";
	char * namedPipe2 = "/tmp/N_pipe2";
 	int fd = open(namedPipe1, O_RDONLY);
	
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


	//setting GPS struct to not NULL
	gettimeofday(&GPS, NULL);
	//getting gps signal
	while(1)
	{
		if(read(fd, &buffer, sizeof(unsigned char)) < 0) //error because recieving zeros I believe
		{
			printf("Can't read n_pipe1...\n");
		}
		//reading from pipeline
		
		bytes = read(fd, &buffer, sizeof(unsigned char)); //reading into buffer and setting to str1
		buffer[1] = '\0'; //just in case
		gettimeofday(&GPS, NULL);
		//printf("Number of bytes read: %d\n", bytes);
		if(bytes == -1)
			printf("No GPS signal being recieved\n");
		//else
			//printf("GPS signal: %c\n", buffer[0]);

	}	
	
	pthread_join(thread0_EventThread, NULL);
	close(fd);
	pthread_exit(NULL);
}





