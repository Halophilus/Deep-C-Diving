# Compiler and flags
CC      := gcc
CFLAGS  := -Wall -Wextra -g -Iinclude -MMD -MP
LDFLAGS :=

# Directories
SRC_DIR := src
INC_DIR := include
OBJ_DIR := build
SERVER_DIR := $(SRC_DIR)/server
CLIENT_DIR := $(SRC_DIR)/client

# Source files
SRCS := $(wildcard $(SRC_DIR)/**/*.c)

# Object files (.c -> .o) and dependency files (.c -> .d)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

# Executables (temporary build names)
SERVER := server
CLIENT := client

# Final locations
SERVER_FINAL := $(SERVER_DIR)/server
CLIENT_FINAL := $(CLIENT_DIR)/client

# Object groups
SERVER_OBJS := $(OBJ_DIR)/server.o $(OBJ_DIR)/waitingroom.o $(OBJ_DIR)/queue.o $(OBJ_DIR)/messenger.o
CLIENT_OBJS := $(OBJ_DIR)/client.o $(OBJ_DIR)/messenger.o

# Default target
all: $(SERVER_FINAL) $(CLIENT_FINAL)

# Build server and move it into server folder
$(SERVER_FINAL): $(SERVER_OBJS)
	$(CC) $^ -o $(SERVER)
	@mkdir -p $(SERVER_DIR)
	mv $(SERVER) $(SERVER_FINAL)

# Build client and move it into client folder
$(CLIENT_FINAL): $(CLIENT_OBJS)
	$(CC) $^ -o $(CLIENT)
	@mkdir -p $(CLIENT_DIR)
	mv $(CLIENT) $(CLIENT_FINAL)

# Pattern rule for building .o files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Include generated dependency files
-include $(DEPS)

# Clean rule
clean:
	rm -rf $(OBJ_DIR) $(SERVER_FINAL) $(CLIENT_FINAL)

.PHONY: all clean
