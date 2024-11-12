#include "TcpIpClient.h"

void error(const char *msg){
    perror(msg);
}

void errorAndExit(const char *msg)
{
    perror(msg);
    exit(0);
}

[[noreturn]] void TcpIpClient::ConnectAndStartListen_Blocked()
{
    //Ожидание подключения к серверу-----------------------------
    _clientStatus = InitConnect;
    do
    {
        cout<<"Connect to...." <<_ipAddress<<":"<<_portNumber<<endl;
        if(_clientStatus == InitConnect)
        {
            sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (sockfd < 0)
                errorAndExit("ERROR opening socket");

            hint.sin_family = AF_INET;
            hint.sin_port = htons(_portNumber);
            inet_pton(AF_INET, _ipAddress.c_str(), &hint.sin_addr);
        }

        //	Connect to the server on the socket
        int connectRes = connect(sockfd, (sockaddr*)&hint, sizeof(hint));
        if (connectRes == -1)
        {
            error("ERROR connecting....");
            _clientStatus = ErrorConnect;
            sleep_for(2000ms);
        }
        else
        {
            _clientStatus = Connected;
            cout << "Connect to " << _ipAddress <<":"<< _portNumber;
        }

        //Ожидание данных от сервера----------------------------------------
        while (_clientStatus == Connected)
        {
            memset(buffer, 0,BUFF_SIZE);
            int bytesReceived = recv(sockfd, buffer, BUFF_SIZE, 0);
            if (bytesReceived < 0)
            {
                error("ERROR reading from socket");
                _clientStatus = ErrorReading;
                break;
            }
            else
            if(bytesReceived == 0){
                error("ERROR server close socket...");
                _clientStatus = ServerClouseSocket;
                break;
            }

            //Успешно приняли данные------------------------------------
            if(pFunctionRawDataConsume != nullptr){
                pFunctionRawDataConsume(buffer, bytesReceived);
            }
        }

        if(_clientStatus == ErrorReading || _clientStatus == ServerClouseSocket)
        {
            close(sockfd);
            _clientStatus = InitConnect;
        }
    } while (true);


    //Запрос серверу-------------------------------------------
    //printf("Please enter the message: ");
    // bzero(buffer,256);
    // // fgets(buffer,255,stdin);
    // memcpy(buffer, "12345aa", 8);
    // n = write(sockfd,buffer,strlen(buffer));
    // if (n < 0)
    //      error("ERROR writing to socket");
    // bzero(buffer,256);
}