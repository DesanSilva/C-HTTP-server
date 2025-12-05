CC = gcc
CFLAGS = -Wall -Wextra -Wno-implicit-fallthrough -O2 -I.
LDFLAGS = -lssl -lcrypto

TARGET = build/c-http-server
SOURCES = $(wildcard src/*.c)
HEADERS = $(wildcard include/*.h)
OBJECTS = $(SOURCES:src/%.c=build/%.o)

$(shell mkdir -p build)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

build/%.o: src/%.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
