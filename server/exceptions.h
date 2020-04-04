//
// Created by zl246 on 2020/4/1.
//

#ifndef HOMEWORK4_EXCEPTIONS_H
#define HOMEWORK4_EXCEPTIONS_H

#include <exception>
#include <cstdio>

struct getaddrinfoException : public std::exception {
    const char *what() const throw() {
        return "getaddrinfo() failed";
    }
};

struct listenException : public std::exception {
    const char *what() const throw() {
        perror ("listen");
        return "listen() failed";
    }
};

struct acceptException : public std::exception {
    const char *what() const throw() {
        perror ("accept");
        return "accept() failed";
    }
};

struct recvException : public std::exception {
    const char *what() const throw() {
        perror ("recv");
        return "recv() failed";
    }
};

struct sendException : public std::exception {
    const char *what() const throw() {
        perror ("send");
        return "send() failed";
    }
};

#endif //HOMEWORK4_EXCEPTIONS_H
