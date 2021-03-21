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

//struct of shared memory for interpolation threads, don't know if works
struct SharedMemory
{
	struct timeval X_before_btn_press;
	struct timeval X1;
	struct timeval X2;
	double Y1; //don't know why it works with doubles
	double Y2;
	double Y_before_btn_press;
};

sem_t sema; //semaphore for whatever
struct timeval GPS; //struct for time of gps
char buffer[2]; //buffer for reading from fd
int p[2]; //simple pipe for printing

//named pipe and print thread
void *Pthread1(void *args)
{
	//printf("entering print thread\n");

	struct SharedMemory print_data;
	
	read(p[0], &print_data, sizeof(struct SharedMemory));
	
	printf("\n--------PRINTING INTERPOLATION---------\n");
	
	//printing actual values
	printf("x1 is: %d:%d\n", print_data.X1.tv_sec, print_data.X1.tv_usec); printf("y1 is: %lf\n", print_data.Y1);
	printf("x2 is: %d:%d\n", print_data.X2.tv_sec, print_data.X2.tv_usec); printf("y2 is: %lf\n", print_data.Y2);
	printf("X coordinate of button press is %d:%d\n", print_data.X_before_btn_press.tv_sec, 		print_data.X_before_btn_press.tv_usec);
	printf("Y coordinate of button press is %lf\n", print_data.Y_before_btn_press);
	
	sem_post(&sema);
	
	pthread_exit(NULL);
}

//thread for interpolating the data from Pthread0
void * childThread(void *args)
{
	//printf("Entering child thread for interpolation...\n");
	
	struct SharedMemory data;
	
	//-------interpolation------
	
	//getting passed button press from thread0
	data.X_before_btn_press = *(struct timeval *)args;
	
	data.Y1 = buffer[0];
	data.X1 = GPS; //<-^ from main thread 
	
	while(data.Y1 == buffer[0])
	{
		//waiting for buffer to update next data point before moving on
	} 
	
	data.Y2 = buffer[0];
	data.X2 = GPS; //same as above but next point 
	
	//finding slopes and doing interpolation
	//finding the milliseconds of the slope
	int slope_milli = ((data.X2.tv_sec - data.X1.tv_sec) * 1000 + ((int)data.X2.tv_usec - (int)data.X2.tv_usec)/1000); 
	
	int slope2_milli = ((data.X_before_btn_press.tv_sec - data.X1.tv_sec) * 1000 + ((int)data.X2.tv_usec - (int)data.X2.tv_usec)/1000);
	
	//finding Y_before_btn_press
	data.Y_before_btn_press = (((data.Y2-data.Y1)*slope_milli) * (slope2_milli)) + data.Y1;


	//------Passing to print thread--------
	//wait for semaphore before starting print process
	sem_wait(&sema);
	
	//writing to SIMPLE pipe
	write(p[1], &data, sizeof(struct SharedMemory)); //passing through pipe
		
	//passing to thread
	pthread_t thread;
	pthread_create(&thread, NULL, Pthread1, NULL);
	pthread_exit(NULL);
}



//thread for receiving real time event
//Will dynamically make child threads for button presses
void *Pthread0()
{
	int fd;
	char * namedPipe2 = "/tmp/N_pipe2";
	fd = open(namedPipe2, O_RDONLY); //reading second named pipe to recieve real time event
	struct timeval buffer_btn; //for passing to child thread
	
	//looping nad reading
	while(1)
	{
		read(fd, &buffer_btn, sizeof(struct timeval));
		
		pthread_t thread;
		pthread_create(&thread, NULL, childThread, &buffer_btn);
	}
	close(fd);
}



int main()
{
	//semaphore init
	sem_init(&sema, 0, 1);
	
	char * namedPipe1 = "/tmp/N_pipe1";
	char * namedPipe2 = "/tmp/N_pipe2";
	char * printPipe = "/tmp/print_pipe";
	
 	pthread_t thread0_EventThread;
 	
 	int fd = open(namedPipe1, O_RDONLY);
	
	//making pipes if not made already
	if(mkfifo(namedPipe1, 0777) == 0)
	{
		printf("N_pipe1 created\n");
	}
	else
	{
		printf("N_pipe1 has already been created...\n");
	}
	
	if(mkfifo(namedPipe2, 0777) == 0)
	{
		printf("N-pipe-2 created\n");
	}
	else
	{
		printf("N-pipe-2 has already been created...\n");
	}
	
	if(mkfifo(printPipe, 0777) == 0)
	{
		printf("Print pipe created\n");
	}
	else
	{
		printf("Print pipe has already been created...\n");
	}
	
	if(pipe(p) < 0)
		printf("simple pipe failed\n");
	
	//creating event thread
	pthread_create(&thread0_EventThread, NULL, Pthread0, NULL);


	//setting GPS struct to not NULL
	gettimeofday(&GPS, NULL);
	
	//getting gps signal
	while(1)
	{
		//reading from pipeline
		
		read(fd, &buffer, sizeof(unsigned char)); //reading into buffer and setting to str1
		buffer[1] = '\0'; //just in case
		gettimeofday(&GPS, NULL);

		//proof of concept for week one and in case of debugging
		/*if(bytes == -1)
			printf("No GPS signal being recieved\n");
		else
			printf("GPS signal: %c\n", buffer[0]);*/

	}	
	
	pthread_join(thread0_EventThread, NULL);
	close(fd);
	pthread_exit(NULL);
}





