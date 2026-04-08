CC=gcc
CFLAGS=-Wall

all: target fuzzer

target: vulnerable_target.c
	$(CC) -fno-stack-protector -z execstack vulnerable_target.c -o target

fuzzer: fuzzer.c
	$(CC) $(CFLAGS) fuzzer.c -o fuzzer

clean:
	rm -f target fuzzer mutated_input.txt CRASH_TRIGGER.txt