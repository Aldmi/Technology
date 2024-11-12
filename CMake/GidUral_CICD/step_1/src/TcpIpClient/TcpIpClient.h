#ifndef GIDURALCLIENT_TCPIPCLIENT_H
#define GIDURALCLIENT_TCPIPCLIENT_H

#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <chrono>
#include <thread>

#include <iostream>
#include "../Shared/Shared.h"


using namespace std;
using namespace std::this_thread;     // sleep_for, sleep_until
using namespace std::chrono_literals; // ns, us, ms, s, h, etc.
using std::chrono::system_clock;

const size_t BUFF_SIZE = 8192;

enum ClientStatus
{
    NotConnect,
    InitConnect,
    Connected,
    ErrorConnect,
    ErrorReading,
    ServerClouseSocket
};


class TcpIpClient
{
private:
    string _ipAddress;
    int _portNumber;

    int sockfd = -1;
    struct sockaddr_in hint;
    Byte buffer[BUFF_SIZE];

    ClientStatus _clientStatus;


public:
    TcpIpClient(string ipAddress, int portNumber){
        _ipAddress = ipAddress;
        _portNumber = portNumber;
        _clientStatus = NotConnect;
        sockfd= -1;
    }

    ~TcpIpClient(){
        if(sockfd > 0){
            close(sockfd);
        }
    }

    ClientStatus GetClientStatus(){ return _clientStatus; }

    int (*pFunctionRawDataConsume)(Byte buffer[BUFF_SIZE], size_t size){nullptr};

    //Выполняет подключение и начинает прослушивать сокет.
    //Операция является блокируюшей.
    //Полученные данные обрабатываются в callback функции.
    [[noreturn]] void ConnectAndStartListen_Blocked();

    bool SendRequest(Byte* buf, size_t size);
};









#endif //GIDURALCLIENT_TCPIPCLIENT_H
