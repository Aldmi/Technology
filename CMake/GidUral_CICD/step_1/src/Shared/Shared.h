#pragma once

#include <cstdint>
#include <cstring>
#include <iostream>

// type support
#include <./cereal/types/map.hpp>
#include <./cereal/types/vector.hpp>
#include <./cereal/types/string.hpp>
#include <./cereal/types/complex.hpp>
#include <./cereal/types/array.hpp>
#include <cereal/archives/json.hpp>
#include <sstream>

using namespace std;

using Byte = unsigned char;

int buffToInteger(Byte* buffer);
unsigned int  buffToUnsignedInteger(Byte* buffer);
short buffToShort(Byte* buffer);
long long fiveBytesToLong(Byte* buffer);
long long sixBytesToLong(Byte* buffer);
int threeBytesToInt(Byte* buffer);
string buffToString(Byte* buffer, int size);

void PrintBuffer(Byte* buffer, int size);

string buffToTimeStampStr(Byte* buffer);


void removeSpecialChars(string& str);


static const Byte strPatternToByte(std::string pattern, std::vector<Byte> *values)
{
    std::istringstream stream(pattern);
    unsigned value;
    while (stream >> std::hex >> value)
    {
        values->push_back(value);
    }
    return 10;
}


//Текущее время
static const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    return buf;
}
