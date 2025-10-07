CC = gcc
CFLAGS = -Wall -Wextra -Iinclude

SRC_DIR = src
INC_DIR = include
BUILD_DIR = build

SRCS = $(SRC_DIR)/main.c \
       $(SRC_DIR)/huffman.c \
       $(SRC_DIR)/txt.c \
       $(SRC_DIR)/img.c \
       $(SRC_DIR)/stats.c  

OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

TARGET = $(BUILD_DIR)/compressor.exe



all: setup $(TARGET)

setup:
	@if not exist "$(BUILD_DIR)" mkdir "$(BUILD_DIR)"
	@echo Build directory ready.

$(TARGET): $(OBJS)
	@echo Linking all modules...
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)
	@echo Build successful! Executable created: $(TARGET)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@echo Compiling $< ...
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	@echo Cleaning build files...
	@if exist "$(BUILD_DIR)\*.o" del /Q "$(BUILD_DIR)\*.o"
	@if exist "$(TARGET)" del /Q "$(TARGET)"
	@echo Clean complete.

rebuild: clean all

