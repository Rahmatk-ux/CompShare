# Compiler and flags
CC = gcc
CFLAGS = -Iinclude -Wall -O2

# Source files
SRC = src\main.c src\huffman.c src\txt.c src\img.c src\stats.c
OBJ = $(SRC:.c=.o)

# Target executable
TARGET = CompShare.exe

# Backend folder
BACKEND_DIR = backend

all: $(TARGET) copy_to_backend

# Build executable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Compile .c to .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Copy executable to backend folder
copy_to_backend: $(TARGET)
	if not exist $(BACKEND_DIR) mkdir $(BACKEND_DIR)
	copy $(TARGET) $(BACKEND_DIR)\$(TARGET)
	@echo Executable copied to $(BACKEND_DIR)\$(TARGET)

# Clean object files and executable
clean:
	del /Q $(OBJ) $(TARGET)
