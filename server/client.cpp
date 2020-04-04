//
// Created by zl246 on 2020/4/1.
//
#include <iostream>
#include <vector>
#include "socket.h"
#include "utility.h"

using namespace std;

void clientServerTest(const char *hostName, const string &request) {
    try{Socket clientLocalSocket(hostName, "12345");
    int clientLocalSocketFD = clientLocalSocket.setupClientAndConnectServer();

    //prepare a request string and send it to the server
    //std::string request("12345678901234567890,12345678901234567890\n");

    // std::cout << "Data to send: " << request.c_str() << std::endl;

    sendData(clientLocalSocketFD, request.c_str(), request.size());

    char responseBuffer[256]; //store the response from the server
    int bufferLen = sizeof(responseBuffer);
    memset(responseBuffer, 0, bufferLen);
    receiveData(clientLocalSocketFD, responseBuffer, bufferLen);
    // std::cout << "Data received: " << responseBuffer << std::endl;

    clientLocalSocket.closeSocket();}
    catch (recvException& rE) {
      cout << "handled" << endl;
    }
}

void runClient(const char *hostName, int delay, int bucket) {
    //use different functions here to achieve different testing purposes

    //build request
    string request = to_string(delay) + "," + to_string(bucket) + "\n";
    clientServerTest(hostName, request);
}

//run with client 127.0.0.1 to connect to the server on the same machine
int main(int argc, char *argv[]) {
    std::cout << "Now the client will start..." << std::endl;
    if (argc != 2) {
        std::cerr << "Usage: client <host name>" << std::endl;
        exit(EXIT_FAILURE);
    }

    const char *hostName = argv[1]; //??? how to decide the memory size at .data??
    std::cout << "Now try connecting to the server..." << std::endl;

    try {
        runClient(hostName, 2, 1);
    }
    catch (std::exception &e) {
        std::cerr << "Exception happened!" << std::endl;
        e.what();
    }

    return 0;
}