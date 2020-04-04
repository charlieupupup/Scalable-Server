#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <sstream>
#include <sys/time.h>
#include <atomic>
#include <unistd.h>

#include "utility.h"
#include "ctpl_stl.h"

#define THREAD_POOL_SIZE (int)100

std::mutex bucketMutex;

std::atomic<int> requestNum{0};

void doDelayLoop(int delayCount) {
    struct timeval start, check;
    double elapsed_seconds;
    gettimeofday(&start, NULL);
    do {
        gettimeofday(&check, NULL);
        elapsed_seconds =
                (check.tv_sec + (check.tv_usec / 1000000.0)) -
                (start.tv_sec + (start.tv_usec / 1000000.0));
    } while (elapsed_seconds < delayCount);
}

void addBucket(std::vector<unsigned long int> &buckets, unsigned long int delayCount, unsigned long int bucketToAdd) {
    //acquire lock
    std::lock_guard<std::mutex> guard(bucketMutex);
    buckets[bucketToAdd] += delayCount;
    //release lock automatically
}

long int readBucket(std::vector<unsigned long int> &buckets, unsigned int bucketIndex) {
    //acquire lock
    std::lock_guard<std::mutex> guard(bucketMutex);
    return buckets[bucketIndex];
}


void calculateThroughput(int waitingSeconds, std::vector<unsigned long int> &buckets) {
    sleep(waitingSeconds);
    double throughput = requestNum / (double) waitingSeconds;
    std::cout << "Note: Throughput is " << throughput << std::endl;

//    std::cout << "Now will print the bucket" << std::endl;
//    sleep(1);
//    //print vector
//    for (int i = 0; i < buckets.size(); i++) {
//        std::cout << i << ": " << buckets[i] << std::endl;
//    }
}


/* As a server, receive the request content, do the requested work and send back response.
 * */
void handleRequest(int clientFD, std::vector<unsigned long int> &buckets) {
    char requestBuffer[256]; //store the two parameters in a client's request
    int bufferLen = sizeof(requestBuffer);
    memset(requestBuffer, 0, bufferLen);

    //receive the request
    receiveData(clientFD, requestBuffer, bufferLen);

    std::string parse(requestBuffer);
    std::stringstream ss(parse);

    //parse the request
//    int pos = parse.find(',');
    int delayCount;
    int bucketToAdd;
    char c; //dump variable
    ss >> delayCount;
    ss >> c;
    ss >> bucketToAdd;

    //process the request: add the number to the specified bucket
    doDelayLoop(delayCount);
    addBucket(buckets, delayCount, bucketToAdd);

    //send back response
    std::string response = std::to_string(readBucket(buckets, bucketToAdd));
    response.push_back('\n');
    sendData(clientFD, response.c_str(), response.size());

    //finish this request, add to counter
    requestNum++;

    close(clientFD);
}

void handleRequestWithThreadPool(int ctplID, int clientFD, std::vector<unsigned long int> &buckets) {
    handleRequest(clientFD, buckets);
}

void runServer(std::vector<unsigned long int> &buckets, int method) {
    const char *portNum = "12345";
    //create a socket for the server to wait for requests from clients
    Socket serverSocket(NULL, portNum);
    serverSocket.setupServer(); //non-blocking

    //a thread to waiting 30 seconds and calculate the average throughput
    std::thread calcThroughput(calculateThroughput, 20, std::ref(buckets));
    calcThroughput.detach();

    //per thread per request strategy
    if (method == 0) {
        while (true) {
            int clientFD = serverSocket.acceptRequest(); //blocking
            std::thread t(handleRequest, clientFD, std::ref(buckets));
            t.detach();
        }
    }

    //using thread pool
    if(method == 1){
        ctpl::thread_pool p(THREAD_POOL_SIZE);
        while (true) {
            int clientFD = serverSocket.acceptRequest(); //blocking
            p.push(handleRequestWithThreadPool, clientFD, std::ref(buckets));
        }
    }
}

int main(int argc, char *argv[]) {
    std::cout << "Now will start the server..." << std::endl;

    if (argc != 3) {
        std::cout << "Usage: server <method #> <# of buckets>" << std::endl;
        exit(EXIT_FAILURE);
    }

    int method = atoi(argv[1]); // 0 for per thread per request, 1 for pre-created request
    int bucketNum = atoi(argv[2]);
    std::cout << "The bucket size will be set to " << bucketNum << std::endl;

    //shared data structure among threads
    std::vector<unsigned long int> buckets;
    buckets.resize(bucketNum, 0);

    try {
        runServer(buckets, method);
    } catch (std::exception &e) {
        std::cerr << "Exceptions happened!" << std::endl;
        e.what();
    }
    return 0;
}
