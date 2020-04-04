//
// Created by zl246 on 2020/4/2.
//

#ifndef HOMEWORK4_UTILITY_H
#define HOMEWORK4_UTILITY_H

#include <sys/time.h>
#include "socket.h"

/* Receive the data using recv(), and handle the partial receive.
 * socketFD: the file descriptor of the connected socket
 * dataBuffer: the const pointer to a data buffer, can be a C array or vector, etc.
 * bufferLen: the maximum length of the data buffer.
 * */
void receiveData(const int socketFD, char *dataBuffer, const int bufferLen) {
    try{
      int receivedLen = 0;
      int receivedTotalLen = 0;
      size_t findResult = std::string::npos;
      while (findResult == std::string::npos) {
          receivedLen = recv(socketFD, &(dataBuffer[receivedTotalLen]), bufferLen, 0);
          if (receivedLen == -1) {
             std::cerr << "From receiveData()" << std::endl;
             throw recvException();
          }
          //TODO: how to handle recv() return 0, i.e., socket closed ???
         receivedTotalLen += receivedLen;

          //check whether received a '\n'
          //if we find a '\n', it means we have received all the data of a request
          std::string temp(dataBuffer);
          findResult = temp.find('\n');
          //TODO: if the transmission is totally corrupted and cannot receive '\n' in the end, how to handle this situation?
      }
    }
    catch (recvException& rE) {
      close(socketFD);
      throw;
    }
} 


/* Send the data, and also handle the partial send.
 * */
void sendData(const int socketFD, const char* dataBuffer, const int dataLen) {
    int sendLen = 0;
    int sendTotalLen = 0;
    int remainedData = dataLen - sendTotalLen;
    while(sendTotalLen < dataLen){
        sendLen = send(socketFD, dataBuffer + sendTotalLen, remainedData, 0);
        if(sendLen == -1){
            std::cerr << "From sendData()" << std::endl;
            throw sendException();
            //close this connection?
        }
        sendTotalLen += sendLen;
        remainedData -= sendLen;
    }
}

#endif //HOMEWORK4_UTILITY_H
