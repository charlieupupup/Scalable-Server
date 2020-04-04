CC=g++
CFLAGS=-ggdb3 -pedantic -Werror -Wall -pthread
EXECUTABLE=test test_long_delay
T=test.cpp

all: $(EXECUTABLE)

test: $(T)  
	$(CC) $(CFLAGS) -o $@ $^

test_long_delay: test_long_delay.cpp
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -rf test *.o *~