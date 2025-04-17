# Compiler and flags
CC := gcc
CFLAGS := -Wall -Wextra -g -Iinclude
LDFLAGS :=

# Directories
SRC_DIR := src
INC_DIR := include
OBJ_DIR := build

CLIENT_SRC := $(SRC_DIR)/client/client.c
SERVER_SRC := $(SRC_DIR)/server/server.c
COMMON_SRCS := $(SRC_DIR)/messenger.c $(SRC_DIR)/queue.c $(SRC_DIR)/waitingroom.c

CLIENT_OBJS := $(OBJ_DIR)/client.o $(OBJ_DIR)/messenger.o $(OBJ_DIR)/queue.o $(OBJ_DIR)/waitingroom.o
SERVER_OBJS := $(OBJ_DIR)/server.o $(OBJ_DIR)/messenger.o $(OBJ_DIR)/queue.o $(OBJ_DIR)/waitingroom.o

# Targets
CLIENT_BIN := client/client
SERVER_BIN := server/server

# Default target
all: $(CLIENT_BIN) $(SERVER_BIN)

# Compile client
$(CLIENT_BIN): $(CLIENT_OBJS)
	@mkdir -p $(dir $@)
	$(CC) $^ -o $@ $(LDFLAGS)

# Compile server
$(SERVER_BIN): $(SERVER_OBJS)
	@mkdir -p $(dir $@)
	$(CC) $^ -o $@ $(LDFLAGS)

# Compile .c -> .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Special cases for nested client/server .c files
$(OBJ_DIR)/client.o: $(CLIENT_SRC)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/server.o: $(SERVER_SRC)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean
clean:
	rm -rf $(OBJ_DIR) $(CLIENT_BIN) $(SERVER_BIN)

.PHONY: all clean
