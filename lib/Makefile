DYNAMIC_LIB = libmaze.dylib
STATIC_LIB = libmaze.a
CC = g++-5 -std=c++11 -pthread -flto -Wall -Wextra -Wpedantic
AR = ar
CFLAGS = -fPIC -Ofast -g3
# CFLAGS = -fPIC -Og -g3
LDFLAGS = -dynamiclib

SRCS = maze.c xorshift.c sidewinder.c
OBJS = $(SRCS:.c=.o)

.PHONY: all
all: $(STATIC_LIB) $(DYNAMIC_LIB)

$(DYNAMIC_LIB): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

$(STATIC_LIB): $(OBJS)
	$(AR) rcs $@ $^

$(SRCS:.c=.d):%.d:%.c
	$(CC) $(CFLAGS) -MM $< >$@
	include $(SRCS:.c=.d)

.PHONY: clean
clean:
	rm -f $(OBJS) $(SRCS:.c=.d) $(DYNAMIC_LIB) $(STATIC_LIB)
