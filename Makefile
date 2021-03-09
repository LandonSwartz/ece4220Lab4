CC = gcc
CFLAGS = #-Wall
LDFLAGS=-lpthread -lrt -lwiringPi
OBJFILES = Lab4.o
TARGET = Lab4
	
$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES) $(LDFLAGS)
clean:
	rm -f $(OBJFILES) $(TARGET) *~
