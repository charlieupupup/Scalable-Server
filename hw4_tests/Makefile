all: server client

server: server.cpp
	g++ -std=c++17 -O3 -o server server.cpp -lpthread

client: client.cpp
	g++ -std=c++17 -O3 -o client client.cpp -lpthread

clean:
	rm -f *~ *.o client server
