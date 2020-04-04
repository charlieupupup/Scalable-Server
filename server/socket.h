//
// Created by zl246 on 2020/3/31.
//

#ifndef HOMEWORK4_SOCKET_H
#define HOMEWORK4_SOCKET_H

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <unistd.h>

#include "exceptions.h"

#define INCOMMING_QUEUE 200

class Socket {
private:
    int socketFD;
    int yes;
    //char remoteIP;
    const char *hostName;
    const char *portNum;
    struct addrinfo hints;
    struct addrinfo *hostInfoList;
    struct addrinfo *firstUsableHostInfo;
    struct sockaddr_storage remoteAddr;
    socklen_t remoteAddrLen;

public:
    /* As a constructor, initialize the struct addrinfo hint for calling getaddrinfo later.
     * If the socket is for a server, the hostname is NULL and set hint.ai_flags = AI_PASSIVE
     * as we've done in ECE 650 potato project.
     * */
    Socket(const char *_hostName, const char *_portNum) : yes(1), hostName(_hostName), portNum(_portNum),
                                                          firstUsableHostInfo(NULL), remoteAddrLen(sizeof(remoteAddr)) {
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        if (_hostName == NULL) { //socket for a server
            hints.ai_flags = AI_PASSIVE;
        }
    }

    void setupServer();

    int acceptRequest();

    int setupClientAndConnectServer();

    //void connectToServer();

    void closeSocket();

    ~Socket() {
        freeaddrinfo(hostInfoList);
    }

};


/* As a server, setup itself and start to listen() to a port, ready for the incoming
 * connection - i.e., first it will getaddrinfo(), then make a socket(), then set the
 * socket reusable, then bind() to a port (here is port 12345), and start to listen().
 * */
void Socket::setupServer() {
    int status = getaddrinfo(hostName, portNum, &hints, &hostInfoList);
    if (status != 0) {
        std::cerr << "Error in getaddrinfo: " << gai_strerror(status) << std::endl;
        throw getaddrinfoException();
    }

    //iterate through the list and find the first usable one, make a socket and bind it
    for (firstUsableHostInfo = hostInfoList; firstUsableHostInfo != NULL; firstUsableHostInfo = hostInfoList->ai_next) {
        if ((socketFD = socket(firstUsableHostInfo->ai_family, firstUsableHostInfo->ai_socktype,
                               firstUsableHostInfo->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }
        if (setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror("setsocketopt");
            continue;
        }
        if (bind(socketFD, firstUsableHostInfo->ai_addr, firstUsableHostInfo->ai_addrlen) == -1) {
            close(socketFD);
            perror("server: bind");
            continue;
        }
        break;
    }

    //check the result
    if (firstUsableHostInfo == NULL) {
        std::cerr << "failed to bind the socket, now retrying" << std::endl;
        throw std::runtime_error("bind failure");
    }

    //start to listen this port
    if (listen(socketFD, INCOMMING_QUEUE) == -1) {
        std::cerr << "Error from setupServer()" << std::endl;
        throw listenException();
    }
}


/* As a server, accept() the incoming connection and return a new socket file descriptor.
 * */
int Socket::acceptRequest() {
    int clientSocketFD;
    clientSocketFD = accept(socketFD, (struct sockaddr *) &remoteAddr, &remoteAddrLen);
    if (clientSocketFD == -1) {
        std::cerr << "Error from acceptRequest()" << std::endl;
        throw acceptException();
    }
    return clientSocketFD;
}


/* As a client, setup itself and ready for send() data - i.e., it will
 * getaddrinfo(), then make a socket(), then connect() to the server.
 *
 * Return the socket file descriptor of the connected socket.
 * */
int Socket::setupClientAndConnectServer() {
    int status = getaddrinfo(hostName, portNum, &hints, &hostInfoList);
    if (status != 0) {
        std::cerr << "Error in getaddrinfo: " << gai_strerror(status) << std::endl;
        throw getaddrinfoException();
    }

    //iterate through the list and find the first usable one, make a socket
    for (firstUsableHostInfo = hostInfoList; firstUsableHostInfo != NULL; firstUsableHostInfo = hostInfoList->ai_next) {
        if ((socketFD = socket(firstUsableHostInfo->ai_family, firstUsableHostInfo->ai_socktype,
                               firstUsableHostInfo->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if ((connect(socketFD, firstUsableHostInfo->ai_addr, firstUsableHostInfo->ai_addrlen) == -1)) {
            //TODO: if there is some bugs, check here... maybe related to the close of the socketFD...
            close(socketFD);
            perror("client: connect");
            continue;
        }

        break;
    }

    //check the result
    if (firstUsableHostInfo == NULL) {
        std::cerr << "failed to bind the socket, now retrying" << std::endl;
        throw std::runtime_error("client: failed to connect to the server");
    }

    return socketFD;
}

/*
void Socket::connectToServer() {
    int status;
    status = connect(socketFD, firstUsableHostInfo->ai_addr, firstUsableHostInfo->ai_addrlen);
    if(status == -1){

    }
}
*/

void Socket::closeSocket() {
    close(socketFD);
}

#endif //HOMEWORK4_SOCKET_H
