CC = gcc
CFLAGS = -Wall -g
SRC = message.c cache.c messagedriver.c
OBJ = message.o cache.o messagedriver.o
EXEC = messagedriver

# Target to compile the program
$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJ)

# Compile each .c file into .o
message.o: message.c message.h
	$(CC) $(CFLAGS) -c message.c -o message.o

cache.o: cache.c message.h
	$(CC) $(CFLAGS) -c cache.c -o cache.o

messagedriver.o: messagedriver.c message.h
	$(CC) $(CFLAGS) -c messagedriver.c -o messagedriver.o

# Target to clean up object files, executable, and reset messages.csv with a header
clean:
	rm -f $(OBJ) $(EXEC)
	# Reset messages.csv with the header
	echo "ID,Timestamp,Sender,Receiver,Content,Delivered" > messages.csv

# Target to run tests
run_tests: $(EXEC)
	./$(EXEC)

# Default target
all: $(EXEC)
