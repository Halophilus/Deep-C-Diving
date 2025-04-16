# Compiler and flags
CC      := gcc
CFLAGS  := -Wall -Wextra -g -Iinclude -MMD -MP
LDFLAGS :=

# Directories
SRC_DIR := src
INC_DIR := include
OBJ_DIR := build

# Source files
SRCS := $(wildcard $(SRC_DIR)/*.c)

# Object files (.c -> .o) and dependency files (.c -> .d)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

# Executables
SERVER := server
CLIENT := client

# Object groups
SERVER_OBJS := $(OBJ_DIR)/server.o $(OBJ_DIR)/waitingroom.o $(OBJ_DIR)/queue.o $(OBJ_DIR)/messenger.o
CLIENT_OBJS := $(OBJ_DIR)/client.o $(OBJ_DIR)/messenger.o

# Default target
all: $(SERVER) $(CLIENT)

# Build server
$(SERVER): $(SERVER_OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

# Build client
$(CLIENT): $(CLIENT_OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

# Pattern rule for building .o files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Include generated dependency files
-include $(DEPS)

# Clean rule
clean:
	rm -rf $(OBJ_DIR) $(SERVER) $(CLIENT)

.PHONY: all clean
