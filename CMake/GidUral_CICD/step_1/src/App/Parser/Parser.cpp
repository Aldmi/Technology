#include "Parser.h"
#include "../ActuatorVm/ActuatorVm.h"

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


vector<ActuatorVm>*  Parser::StartParserPipe(
        Byte* buffer,
        size_t bufSize,
        NpkpValidator* npkpValidator)
{
    //PrintBuffer(buffer, bufSize);
    //1. Валидация
    auto validationError = Validate(buffer, bufSize);
    if (!validationError.empty())
    {
        cout << "Validation WaitingCorrectPacket" << validationError;
        return nullptr;//return new GidUralProtocolAbstract(validationError);
    }

    //2. Проверка NKPK
    auto packetSequenceNumber = buffToUnsignedInteger(buffer + 6);
    auto npkpStatus= npkpValidator->Check_NPKP(packetSequenceNumber);
    if (npkpStatus != Ok)
    {
        return nullptr;
    }

    //3. Создание объекта по типу пакета
    vector<ActuatorVm>* pVector = nullptr;
    auto packetType = buffer[13];
    switch (packetType)
    {
        case 0x11: //Текущее состояние всех ИУ
            pVector= GidUralProtocolAbstract::CreateActuatorVm_Type_11(buffer, bufSize);
            break;

        case 0x13: //Текущее состояние одиночного ИУ
            pVector= GidUralProtocolAbstract::CreateActuatorVm_Type_13(buffer, bufSize);
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
    return pVector;
}


string Parser::Validate(Byte* buffer, size_t bufSize)
{
    if (buffer[0] != 0xDB ||
        buffer[1] != 0xDB)
    {
        return "Header error!";
    }

    auto packetType = buffer[13];
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


    auto packetSize = buffToInteger(buffer + 2);
    //Текущее состояние одиночного ИУ
    if(packetType == 0x13)
    {
        if (packetSize != bufSize)
        {
            return "Packet size error!";
        }
    }

    return string();
}

////Фильтрует сырые данные.
////1. оcтаекает timeStamp
//bool Parser::Filter(Byte* buffer)
//{
//    auto packetType = buffer[13];
//    switch (packetType)
//    {
//        case 0x11: //Текущее состояние всех ИУ
//
//            break;
//        default:
//            return true;
//    }
//
//}
//
//


