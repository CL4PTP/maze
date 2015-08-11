LR ?= love
LIBMAZE = libmaze.dylib

.PHONY: all
all: release
	$(LR) .

.PHONY: release
release:
	cd lib && cargo build --release
	ln -s lib/target/release/$(LIBMAZE) ./lib/$(LIBMAZE)

.PHONY: debug
debug:
	cd lib && cargo build
	ln -s lib/target/debug/$(LIBMAZE) ./lib/$(LIBMAZE)

.PHONY: all clean
clean:
	cd lib && cargo clean
	rm -f ./lib/$(LIBMAZE)
