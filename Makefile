#2014.09.01 created by dancal.

CC = g++

TARGET=./bin/wp_log_worker

INCLUDES=-I./include
CFLAGS=-std=c++11 -g -Wall 
#CFLAGS+= -Wextra -Werror -pipe
CFLAGS+=$(INCLUDES)

LIBS=-lm -lgearman 
LFLAGS=-L/usr/lib64

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = $(patsubst %.c, %.o, $(wildcard src/*.c))
HEADERS = $(wildcard include/*.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LIBS) -o $@ $(LFLAGS)

clean:
	-rm -f src/*.o
	-rm -f $(TARGET)
