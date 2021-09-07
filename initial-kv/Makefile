%.o: %.c
	$(CC) -c -o $@ $^ -Werror -Wall -Wpedantic

kv: kv.o backend.o
	$(CC) -o $@ $^

all: kv

clean:
	rm -f kv kv.o backend.o