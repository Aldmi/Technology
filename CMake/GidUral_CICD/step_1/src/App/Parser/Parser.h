#ifndef GIDURALCLIENT_PARSER_H
#define GIDURALCLIENT_PARSER_H


#include "../../Shared/Shared.h"
#include "../GidUralProtocol/GidUralProtocol.h"
#include "../NpkpValidator/NpkpValidator.h"

using namespace std;

class Parser {
public:
    static vector<Byte>* accumulateRawBuffer(Byte* buffer, size_t bufSize);
    static  vector<ActuatorVm>* StartParserPipe(Byte* buffer, size_t bufSize, NpkpValidator* npkpValidator);
    static vector<vector<Byte>> SplitRawBuffer(Byte* buffer, size_t bufSize);

private:
    static string Validate(Byte* buffer, size_t bufSize);
    //static bool Filter(Byte* buffer);

    static vector<Byte> accumulator;
    static int requiredNumberBytes;      //Необходимое кол-во байт (для 0x11 типа)
    //static int lastTimeStampUt;          //Последнее сохраненное значение времени (для 0x11 пакета)
};

#endif //GIDURALCLIENT_PARSER_H
