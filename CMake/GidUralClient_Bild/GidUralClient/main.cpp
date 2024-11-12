#include <iostream>
#include "Shared/Shared.h"
#include "App/GidUralProtocol/GidUralProtocol.h"
#include "App/Parser/Parser.h"
#include "TcpIpClient/TcpIpClient.h"
#include "Tests/MoqData.h"
#include "Settings/Settings.h"
#include "Kafka/KafkaProducer/KafkaProducer.h"

static const string GidId = "010";

static KafkaProducer* kafkaProducerP= nullptr;
static TcpIpClient* tcpIpClientP = nullptr;
static NpkpValidator* npkpValidatorP = nullptr;

int RetransmissionRequest(int packetSequenceNumber){
    //tcpIpClientP->
    return 0;
}


int RawDataHandler(Byte buffer[], size_t size) {
    //1. Накопить в буфере все ожидаемые данные.
    auto accumulatorP= Parser::accumulateRawBuffer(buffer, size);
    if(accumulatorP != nullptr)
    {
        std::cout << std::string (100, '*') << endl;
        std::cout << "accumulated bytes= " << accumulatorP->size() << endl;

        //2. Разбить buffer на массив массивов (array<Byte>). Выделяем секции начиная с 0xDB 0xDB.
        auto splitter = Parser::SplitRawBuffer(accumulatorP->data(), accumulatorP->size());
        std::cout << "splited section number= " << splitter.size() << endl;

        //3. Запуск конвеера обработки для каждой секции
//        for(auto subBuf : splitter)
//        {
//            auto objP = Parser::StartParserPipe(subBuf.data(), subBuf.size(), GidId, npkpValidatorP);
//            if(objP != nullptr && objP->filter_passed)
//            {
//                auto report = objP->SerializeToJson();
//                //4.отправить report
//                removeSpecialChars(report);
//                kafkaProducerP->SendMessage(report);
//                cout << endl << report << endl;//DEBUG
//            }
//            delete objP;
//        }
    }
    return 0;
}


int RawDataConsumer_Invoker(Byte buffer[], size_t size) {

    //cout <<endl<<"Входной буфер:";//DEBUG
    //PrintBuffer(buffer, size);
    return RawDataHandler(buffer, size);
}


int main(int argc, char* argv[]) {
//Получить настройки-----------------------------------------------------------------------
    if(Settings::CreateSettingsFrom_Cmd(argc, argv) < 0)
        return -1;

//Создание kafkaProdusser-----------------------------------------------------------
    string errstr;
    ExampleDeliveryReportCb ex_dr_cb;
    kafkaProducerP= KafkaProducer::CreteProducer(Settings::kafka_addr, Settings::kafka_topic, ex_dr_cb, errstr);

//Создание TcpIpClient---------------------------------------------------------------
    string ipAddress;
    int portNumber;
//     ipAddress= "localhost";
//     portNumber= 8099;
//     ipAddress= "10.35.33.95";
//     portNumber= 9220;
    Settings::ParseTcpAddress(ipAddress, portNumber);
    tcpIpClientP= new TcpIpClient(ipAddress, portNumber);

//Создание NpkpValidator-----------------------------------------------------------------
    npkpValidatorP = new NpkpValidator();
    npkpValidatorP->pFunctionRetransmissionRequest = RetransmissionRequest;


//Запуск TcpIpClient Consumer------------------------------------------------------------
    tcpIpClientP->pFunctionRawDataConsume = RawDataConsumer_Invoker;
    tcpIpClientP->ConnectAndStartListen_Blocked();




//DEBUG Moq Data------------------------------------------------------------------------
//    for(auto buf : sniffData_type_13h)
//    {
//        std::vector<Byte> bufVector;
//        strPatternToByte(buf, &bufVector);
//        return RawDataHandler(bufVector.data(), bufVector.size());
//    }

    char str1[100];
    cin.getline(str1, 100);

    tcpIpClientP->~TcpIpClient();
    if(kafkaProducerP != nullptr){
        delete kafkaProducerP;
    }

    return 0;
}
