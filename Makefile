CC = gcc
CFLAGS = -Wall -Wextra -O2 -I.
LDFLAGS = -lssl -lcrypto

TARGET = tacrine
SOURCES = src/main.c src/io.c src/socket.c src/ssl.c src/requests.c src/server.c
OBJECTS = $(SOURCES:%.c=output/%.o)
HEADERS = include/types.h include/methods.h include/errors.h
OUTPUT_DIR = output

$(shell mkdir -p $(OUTPUT_DIR))

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

output/%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OUTPUT_DIR)
	rm -r $(OBJECTS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run

test: test.c
	gcc -o test test.c
	./test
	rm test
