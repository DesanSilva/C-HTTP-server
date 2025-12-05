CC = gcc
CFLAGS = -Wall -Wextra -O2 -I.
LDFLAGS = -lssl -lcrypto

TARGET = tacrine
SOURCES = main.c io.c socket.c ssl.c requests.c server.c
OBJECTS = $(SOURCES:%.c=output/%.o)
HEADERS = types.h methods.h
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
