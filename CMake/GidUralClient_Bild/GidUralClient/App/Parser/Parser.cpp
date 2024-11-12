#include "Parser.h"

vector<Byte> Parser::accumulator={};
int Parser::requiredNumberBytes = 0; //Кол-во ожидаемых байт, которые нужно накопить в accumulator.



//Копим данные в аккумуляторе.
//Если ожидаемых данных больше чем пришло -> копим, возвращаем nullptr
//Если ожидаемых данных меньше чем пришло -> возвращаем accumulator
vector<Byte> *Parser::accumulateRawBuffer(Byte *buffer, size_t bufSize)
{
    //Если получен первый пакет с НЕ валидным заголовком.
    if(requiredNumberBytes == 0 && buffer[0] != 0xDB && buffer[1] != 0xDB) {
        std::cerr << "first packet not valid= " << std::hex << (int)buffer[0] <<" " << std::hex << (int)buffer[1] << endl;
        return nullptr;
    }

    //Получили новый пакет
    if(buffer[0] == 0xDB && buffer[1] == 0xDB)
    {
        requiredNumberBytes = buffToInteger(buffer + 2);
        accumulator.clear();
    }

    accumulator.insert(end(accumulator), buffer, buffer+bufSize); //Добавить в конец буфера
    auto s= accumulator.size();//DEBUG
    if(accumulator.size() >= requiredNumberBytes)
    {
        requiredNumberBytes=0;
        return &accumulator;
    }
    return nullptr;
}



//Разбор буфера на секции 0xDB0xDB.....0xDB0xDB......
//Если буфер не содержит маркер 0xDB0xDB, то вернуть пустую коллекцию
vector<vector<Byte>> Parser::SplitRawBuffer(Byte* buffer, size_t bufSize)
{
    //Выделить подмассив от 0xDB0xDB до следующей секции 0xDB0xDB
    vector<vector<Byte>> splitter = vector<vector<Byte>>();
    Byte* first= nullptr;
    for (int i= 0; i < bufSize-1; i++)
    {
        if(buffer[i] == 0xDB && buffer[i+1] == 0xDB)
        {
            if(first == nullptr) {
                first = buffer + i;
            }
            else {
                vector<Byte> subBuf = vector<Byte>(first, buffer + i);
                splitter.push_back(subBuf);
                first= buffer + i;
            }
        }
        //Найти конец массива
        if(i + 2 == bufSize)
        {
            //Если нашли начальный маркер 0xDB0xDB
            if(first != nullptr)
            {
                vector<Byte> subBuf = vector<Byte>(first, buffer + i + 2);
                splitter.push_back(subBuf);
            }
        }
    }
    return splitter;
}


GidUralProtocolAbstract* Parser::StartParserPipe(
        Byte* buffer,
        size_t bufSize,
        const string& gidIdExpected,
        NpkpValidator* npkpValidator)
{
    //PrintBuffer(buffer, bufSize);
    //1. Валидация
    auto validationError = Validate(buffer, bufSize, gidIdExpected);
    if (!validationError.empty())
    {
        cout << "Validation WaitingCorrectPacket" << validationError;
        return nullptr;//return new GidUralProtocolAbstract(validationError);
    }

    //2. Проверка NKPK
    auto packetSequenceNumber = buffToInteger(buffer + 6);
    auto npkpStatus= npkpValidator->Check_NPKP(packetSequenceNumber);
    if (npkpStatus != Ok)
    {
        return nullptr;
    }

    //3. Создание объекта по типу пакета
    auto packetType = buffer[13];
    GidUralProtocolAbstract* objP = nullptr;
    switch (packetType)
    {
        case 0x11: //Текущее состояние всех ИУ
            objP= new CurrentStateAllActuators(buffer, bufSize);
            break;

        case 0x13: //Текущее состояние одиночного ИУ
            objP= new CurrentStateSingleActuator(buffer, bufSize);
            break;

        case 0x14: //завершение работы ГМ ГИД
            cout<<"COMMAND END WORK FROM GM GID"<<endl;
            break;

        case 0x15: //потеря связи с ГМ ГИД
            cout<<"COMMAND LOST CONNECT FROM GID"<<endl;
            break;

        case 0x16: //завершение работы Утилиты
            cout<<"COMMAND END WORK UTILITY"<<endl;
            break;

        case 0x17: //запрет соединения для Клиента
            cout<<"COMMAND BLOCKING CONNECTION FOR CLIENT"<<endl;
            break;

        case 0x18: //Пакет «KeepAlive»
            cout<<"COMMAND KeepAlive FOR CLIENT"<<endl;
            break;
    }
    return objP;
}


string Parser::Validate(Byte* arr, size_t arrSize, const string& gidIdExpected)
{
    if (arr[0] != 0xDB ||
        arr[1] != 0xDB)
    {
        return "Header error!";
    }

    auto packetType = arr[13];
    switch (packetType)
    {
        case 0x11: //Текущее состояние всех ИУ
        case 0x13: //Текущее состояние одиночного ИУ
        case 0x14: //завершение работы ГМ ГИД
        case 0x15: //потеря связи с ГМ ГИД
        case 0x16: //завершение работы Утилиты
        case 0x17: //запрет соединения для Клиента
        case 0x18: //Пакет «KeepAlive»
            break;
        default:
            return "Type packet error!";
    }

//    auto gidId =  buffToString(arr + 10, 3);
//    if(gidId != gidIdExpected){
//        return "gidId не совпал";
//    }


    auto packetSize = buffToInteger(arr + 2);
    //Текущее состояние одиночного ИУ
    if(packetType == 0x13)
    {
        if (packetSize != arrSize)
        {
            return "Packet size error!";
        }
    }

//    //Текущее состояние всех ИУ
//    if(packetType == 0x11)
//    {
//        short countActuators= buffToShort(arr + CurrentStateAllActuators::offsetToCountActuators);
//        auto expectedLength= countActuators * Actuator::blockSize + CurrentStateAllActuators::offsetToArrayActuators;
//        if(expectedLength != packetSize){
//            return "Ожидаемая длина секции массива ИУ";
//        }
//    }

//    Byte actuatorTypeSize = arr[18];
//    if (actuatorTypeSize != 50 && actuatorTypeSize != 31) {
//        return "Размер структуры данных  не верно задан (не 50 и не 31 байт)";
//    }

    return string();
}



