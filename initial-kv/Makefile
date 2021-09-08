CFLAGS = -Werror -Wall -Wpedantic -fsanitize=address
LDFLAGS = -fsanitize=address

%.o: %.c
	$(CC) -c -o $@ $^ $(CFLAGS)

kv: kv.o backend.o dll.o
	$(CC) -o $@ $^ $(LDFLAGS)

all: kv

clean:
	rm -f kv kv.o backend.o