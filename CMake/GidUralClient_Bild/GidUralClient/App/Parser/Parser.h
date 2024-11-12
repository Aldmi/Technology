#ifndef GIDURALCLIENT_PARSER_H
#define GIDURALCLIENT_PARSER_H


#include "../../Shared/Shared.h"
#include "../GidUralProtocol/GidUralProtocol.h"
#include "../NpkpValidator/NpkpValidator.h"

using namespace std;

class Parser {
public:
    static vector<Byte>* accumulateRawBuffer(Byte* buffer, size_t bufSize);
    static GidUralProtocolAbstract* StartParserPipe(Byte* buffer, size_t bufSize, const string& gidIdExpected, NpkpValidator* npkpValidator);
    static vector<vector<Byte>> SplitRawBuffer(Byte* buffer, size_t bufSize);

private:
    static string Validate(Byte* arr, size_t arrSize, const string& gidIdExpected);


    static vector<Byte> accumulator;
    static int requiredNumberBytes;      //Необходимое кол-во байт (для 0x11 типа)
};

#endif //GIDURALCLIENT_PARSER_H
