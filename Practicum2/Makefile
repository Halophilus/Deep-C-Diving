# Compiler and flags
CC      := gcc
CFLAGS  := -Wall -Wextra -g -Iinclude -MMD -MP
LDFLAGS :=

# Directories
SRC_DIR := src
INC_DIR := include
OBJ_DIR := build

# Find all .c files recursively
SRCS := $(shell find $(SRC_DIR) -name "*.c")

# Generate corresponding .o and .d files in the build/ directory
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

# Executables
SERVER := server
CLIENT := client

# Explicitly set object dependencies
SERVER_OBJS := $(OBJ_DIR)/server/server.o $(OBJ_DIR)/waitingroom.o $(OBJ_DIR)/queue.o $(OBJ_DIR)/messenger.o
CLIENT_OBJS := $(OBJ_DIR)/client/client.o $(OBJ_DIR)/messenger.o

# Default target
all: $(SERVER) $(CLIENT)

# Build server
$(SERVER): $(SERVER_OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

# Build client
$(CLIENT): $(CLIENT_OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

# Pattern rule to build .o from .c (mirrors directory structure)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Include auto-generated dependency files
-include $(DEPS)

# Clean up everything
clean:
	rm -rf $(OBJ_DIR) $(SERVER) $(CLIENT)

.PHONY: all clean
