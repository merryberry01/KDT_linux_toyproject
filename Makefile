CC = gcc
CFLAGS = -g -Wall

TARGET_NAME = toy_system
TARGET_DIR = ./bin/
BUILD_DIRS = ./ ./system/ ./ui/ ./web_server/

# TARGET: ./bin/toy_system
# SRCS: ./system/system_server.c ... ./web/web_server.c ./main.c
# OBJS: ./system/system_server.o ... ./web/web_server.o ./main.o
TARGET = $(addprefix $(TARGET_DIR), $(TARGET_NAME))
SRCS = $(foreach dir, $(BUILD_DIRS), $(wildcard $(dir)*.c))
OBJS = $(SRCS:.c=.o)

# INCLUDE_DIRS: -I./ -I./system/ -I./ui/ -I./web/
INCLUDE_DIRS = $(foreach dir, $(BUILD_DIRS), -I $(dir))

all: $(TARGET)

$(TARGET): $(OBJS)
	#make ./bin/ directory
	#if it already exists, continue building process
	@ mkdir ./bin 2> /dev/null || true
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) -c $< -o $@

clean:
	@ rm -rf $(TARGET) $(OBJS)
