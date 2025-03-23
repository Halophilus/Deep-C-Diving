CC = gcc
CFLAGS = -Wall -g
SRC = message.c message_test.c
OBJ = message.o message_test.o
EXEC = message_test

# Target to compile the program
$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJ)

# Target to clean up object files, executable, and reset messages.csv with a header
clean:
	rm -f $(OBJ) $(EXEC)
	# Reset messages.csv with the header
	> messages.csv
	echo "ID,Timestamp,Sender,Receiver,Content,Delivered" > messages.csv

# Target to run tests
run_tests: $(EXEC)
	./$(EXEC)

# Default target
all: $(EXEC)

