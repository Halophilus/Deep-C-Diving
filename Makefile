# Compiler and flags
CC      := gcc
CFLAGS  := -Wall -Wextra -g -Iinclude -MMD -MP
ifdef DEBUG
    CFLAGS += -DDEBUG
endif
LDFLAGS :=

# Directories
SRC_DIR := src
INC_DIR := include
OBJ_DIR := build

# Source files
COMMON_SRCS := $(SRC_DIR)/messenger.c $(SRC_DIR)/queue.c $(SRC_DIR)/waitingroom.c
CLIENT_SRC  := $(SRC_DIR)/client/client.c
SERVER_SRC  := $(SRC_DIR)/server/server.c
DRIVER_SRC  := $(SRC_DIR)/concurrency_driver.c

# Object files
COMMON_OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(COMMON_SRCS))
CLIENT_OBJ  := $(OBJ_DIR)/client.o
SERVER_OBJ  := $(OBJ_DIR)/server.o
DRIVER_OBJ  := $(OBJ_DIR)/concurrency_driver.o

# Executables
CLIENT_EXE := client/rfs
SERVER_EXE := server/server
DRIVER_EXE := concurrency_driver

# Default target
all: reset $(CLIENT_EXE) $(SERVER_EXE) $(DRIVER_EXE)

# Build client
$(CLIENT_EXE): $(CLIENT_OBJ) $(COMMON_OBJS)
	@mkdir -p $(dir $@)
	$(CC) $^ -o $@ $(LDFLAGS)

# Build server
$(SERVER_EXE): $(SERVER_OBJ) $(COMMON_OBJS)
	@mkdir -p $(dir $@)
	$(CC) $^ -o $@ $(LDFLAGS)

# Build concurrency driver
$(DRIVER_EXE): $(DRIVER_OBJ)
	$(CC) $^ -o $@ $(LDFLAGS)

# Pattern rule for building .o files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Special cases for nested sources
$(OBJ_DIR)/client.o: $(SRC_DIR)/client/client.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/server.o: $(SRC_DIR)/server/server.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/concurrency_driver.o: $(SRC_DIR)/concurrency_driver.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Include generated dependency files
-include $(wildcard $(OBJ_DIR)/*.d)

# Clean rule
clean:
	rm -rf $(OBJ_DIR) $(CLIENT_EXE) $(SERVER_EXE) $(DRIVER_EXE)
	reset

# Reset rule (restores test file defaults)
reset:
	find server -maxdepth 1 -type f -name "*.txt" -delete
	find client -maxdepth 1 -type f -name "*.txt" -delete
	cp server/backup/*.txt server/
	cp -a client/backup/ client/

.PHONY: all clean reset
