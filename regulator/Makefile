# Cross-compile Makefile for aarch64
CROSS ?= aarch64-linux-gnu-
CC    := $(CROSS)gcc

SRCS := main.c setup.c common.c regulators.c
OBJS := $(SRCS:.c=.o)
TARGET := etm_regulator

CFLAGS := -Wall -Wextra -O2 -g -I. 
LDFLAGS := -lpthread -no-pie -static

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET)
