CC ?= gcc
CFLAGS ?= -std=c11 -O2 -Wall -Wextra -Wpedantic
INCLUDES = -Iinclude
ifeq ($(OS),Windows_NT)
  SOCKET_LIBS = -lws2_32
else
  SOCKET_LIBS =
endif
.PHONY: all test integration-test clean
all: server device test_protocol
server: src/server_main.c src/logger.c src/protocol.c src/socket_utils.c
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@ $(SOCKET_LIBS)
device: src/device_main.c src/config.c src/sensor.c src/protocol.c src/socket_utils.c
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@ $(SOCKET_LIBS)
test_protocol: tests/test_protocol.c src/protocol.c src/config.c
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@
test: test_protocol
	./test_protocol
integration-test: server
	python3 tests/test_stream.py
clean:
	rm -f server device test_protocol *.exe *.o
