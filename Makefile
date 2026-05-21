CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2

SRCS = main.c utils.c fs.c
OBJS = $(SRCS:.c=.o)
TARGET = terminal_simulator

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
