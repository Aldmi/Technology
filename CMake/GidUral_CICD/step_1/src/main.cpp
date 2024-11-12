#include "main.h"


static KafkaProducer* pKafkaProducer= nullptr;
static TcpIpClient* pTcpIpClient = nullptr;
static NpkpValidator* pNpkpValidator = nullptr;
bool PermissionRetransmissionRequest = false;  //чтобы не заспамить RetransmissionRequest запросами. Разрешаем отправку только если пришел новый пакет.


int main(int argc, char* argv[]) {
    cout<< "Ver - " <<VERSION<<endl;
//Получить настройки----------------------------------------------------------------
    if(Settings::CreateSettingsFrom_Env() < 0)
        return -1;


//Создание kafkaProdusser-----------------------------------------------------------
    string errstr;
    ExampleDeliveryReportCb ex_dr_cb;
    pKafkaProducer= KafkaProducer::CreteProducer(Settings::kafka_addr, ex_dr_cb, errstr);

//Создание TcpIpClient---------------------------------------------------------------
    string ipAddress;
    int portNumber;
//     ipAddress= "localhost";
//     portNumber= 8099;
//     ipAddress= "10.35.33.95";
//     portNumber= 9220;
    Settings::ParseTcpAddress(ipAddress, portNumber);
    pTcpIpClient= new TcpIpClient(ipAddress, portNumber);

//Создание NpkpValidator-----------------------------------------------------------------
    pNpkpValidator = new NpkpValidator();
    pNpkpValidator->pFunctionRetransmissionRequest = RetransmissionRequest;

//Запуск TcpIpClient Consumer------------------------------------------------------------
    pTcpIpClient->pFunctionRawDataConsume = RawDataConsumer_Invoker;
    pTcpIpClient->ConnectAndStartListen_Blocked();


//DEBUG Moq Data------------------------------------------------------------------------
//    for(auto buf : sniffData_type_11h_8_actuators)
//    {
//        std::vector<Byte> bufVector;
//        strPatternToByte(buf, &bufVector);
//        RawDataHandler(bufVector.data(), bufVector.size());
//    }
//DEBUG Moq Data------------------------------------------------------------------------

    //DEBUG
//    auto n= NpkpValidator();
//    auto h=n.Check_NPKP(1);
    //DEBUG


    pTcpIpClient->~TcpIpClient();
    if(pKafkaProducer != nullptr){
        delete pKafkaProducer;
    }

    return 0;
}


//Получить данные от TCP socket
int RawDataConsumer_Invoker(Byte buffer[], size_t size)
{
    //cout <<endl<<"Входной буфер:";//DEBUG
    //PrintBuffer(buffer, size);
    return RawDataHandler(buffer, size);
}


//Обработчик данных от TCP socket
int RawDataHandler(Byte buffer[], size_t size)
{
    //Получили новый пакет (в нем могут пакеты с валидным NPKP). Разрешим отправку RetransmissionRequest.
    PermissionRetransmissionRequest = true;

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
        for(auto subBuf : splitter)
        {
            auto pVectorVm = Parser::StartParserPipe(subBuf.data(), subBuf.size(), pNpkpValidator);
            if(pVectorVm != nullptr)
            {
                SendByKafka(pVectorVm);
                delete pVectorVm;
                pVectorVm = nullptr;
            }
        }
    }
    return 0;
}


//Запрос на повторную передачу потерянных пакетов
int RetransmissionRequest(int packetSequenceNumber)
{
    if(!PermissionRetransmissionRequest)
        return -1;

    vector<Byte> vector({
                            0xDA, 0xDA,                                //1..2   Служебное поле (DADAh)
                            0x16, 0x00, 0x00, 0x00,                    //3..6   Размер пакета
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //7..13  Резерв
                            0x01,                                      //14     Тип пакета 01h – запрос на повторную передачу потерянных пакетов
                            0x00, 0x00, 0x00, 0x00                     //15..18 Резерв
                        });
    auto packetSequenceNumberArr= static_cast<Byte*>(static_cast<void*>(&packetSequenceNumber));
    vector.insert(end(vector), packetSequenceNumberArr, packetSequenceNumberArr+4); //19..22 Номер первого потерянного пакета
    auto res=pTcpIpClient->SendRequest(vector.data(), vector.size());
    if(res){
        PermissionRetransmissionRequest = false;
        cout << "RetransmissionRequest send" << endl;
    }
    return 0;
}


//Отправить данные в kafka
void SendByKafka(vector<ActuatorVm>* pVectorVm)
{
    for (int i = 0; i < pVectorVm->size(); ++i)
    {
        auto pActuatorVm= pVectorVm->data() + i;
        auto report = pActuatorVm->SerializeToJson();
        removeSpecialChars(report);
        string esr_id=  "gid_" + std::to_string(pActuatorVm->esr_id);
        std::map<std::string, std::string> mapHeaders {
                {"esr_id", esr_id}
        };
        pKafkaProducer->SendMessage(esr_id, report, mapHeaders);
        cout << endl << report << endl;//DEBUG
    }
}
