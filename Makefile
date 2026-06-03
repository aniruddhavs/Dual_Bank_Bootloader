CC = gcc
CFLAGS = -Wall -Wextra -g

test_crc32: tests/test_crc32.c lib/crc32.c
	$(CC) $(CFLAGS) -o test_crc32 tests/test_crc32.c lib/crc32.c
	./test_crc32

test_cobs: tests/test_cobs.c lib/cobs.c
	$(CC) $(CFLAGS) -o test_cobs tests/test_cobs.c lib/cobs.c
	./test_cobs

clean:
	rm -f test_crc32 test_cobs