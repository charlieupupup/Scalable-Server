#include <iostream>
#include <vector>
#include <thread>
#include <ctime>
#include <chrono>
#include <limits.h>
#include "../server/socket.h"
#include "../server/utility.h"

using namespace std;

void runClient(const string &hostname, int delay, int bucket)
{
  try{
    char *host = new char[hostname.size() + 1];
    strcpy(host, hostname.c_str());
    //build request
    string request = to_string(delay) + "," + to_string(bucket) + "\n";
    Socket clientLocalSocket(host, "12345");

    int clientLocalSocketFD = clientLocalSocket.setupClientAndConnectServer();

    // std::cout << "Data to send: " << request.c_str() << std::endl;

    sendData(clientLocalSocketFD, request.c_str(), request.size());

    char responseBuffer[256]; //store the response from the server
    int bufferLen = sizeof(responseBuffer);
    memset(responseBuffer, 0, bufferLen);
    receiveData(clientLocalSocketFD, responseBuffer, bufferLen);
    // std::cout << "Data received: " << responseBuffer << std::endl;

    clientLocalSocket.closeSocket();
    delete[] host;
  } catch (recvException& rE) {
    rE.what();
      cout << "now conituing testing" << endl;
    }
}

void th(int th_num, const string &host, int delay, int bucket)
{
    vector<thread> ths;
    for (int i = 1; i <= th_num; i++)
    {
        
        int delay_ran = rand() % delay + 1;
        ths.emplace_back(runClient, host, delay_ran, bucket);
    }

    for (auto &th : ths)
    {
        th.join();
    }
}

//different bucket size
int main(int argc, char *argv[])
{
  srand(time(NULL));
    if (argc != 2)
    {
        cout << "Usage: ./test <# of threads>" << endl;
        exit(EXIT_FAILURE);
    }

    int threadNum = atoi(argv[1]);

    string host("127.0.0.1");
    int delay = 3, bucket = 1;

    //runClient(hostname, delay, bucket);

    th(threadNum, host, delay, bucket);

    return 0;
}