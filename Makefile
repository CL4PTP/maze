LR ?= love

.PHONY: all
all: lib/libmaze.so
	$(LR) .

lib/libmaze.so:
	$(MAKE) -C lib

.PHONY: all clean
clean:
	$(MAKE) -C lib clean
