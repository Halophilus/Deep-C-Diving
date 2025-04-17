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

# Source files (recursive wildcard)
SRCS := $(shell find $(SRC_DIR) -name '*.c')

# Object files
OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
DEPS := $(OBJS:.o=.d)

# Executables
SERVER := $(SERVER_DIR)/server
CLIENT := $(CLIENT_DIR)/client

# Object groups (based on relative paths)
SERVER_OBJS := $(OBJ_DIR)/server/server.o \
               $(OBJ_DIR)/waitingroom.o \
               $(OBJ_DIR)/queue.o \
               $(OBJ_DIR)/messenger.o

CLIENT_OBJS := $(OBJ_DIR)/client/client.o \
               $(OBJ_DIR)/messenger.o

# Default target
all: $(SERVER) $(CLIENT)

# Build server
$(SERVER): $(SERVER_OBJS)
	@mkdir -p $(dir $@)
	$(CC) $^ -o $@

# Build client
$(CLIENT): $(CLIENT_OBJS)
	@mkdir -p $(dir $@)
	$(CC) $^ -o $@

# Rule for building .o files (handles nested .c paths)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Include dependency files
-include $(DEPS)

# Clean rule
clean:
	rm -rf $(OBJ_DIR) $(SERVER) $(CLIENT)

.PHONY: all clean
