
#include "Shared.h"
#include <iomanip>


/// <summary>
/// 4 байта в int
/// </summary>
int buffToInteger(Byte* buffer)
{
	return *reinterpret_cast<int*>(buffer);
}

/// <summary>
/// 4 байта в unsigned int
/// </summary>
unsigned int buffToUnsignedInteger(Byte* buffer)
{
    return *reinterpret_cast<unsigned int*>(buffer);
}


/// <summary>
/// 2 байта в short
/// </summary>
short buffToShort(Byte* buffer)
{
	return *reinterpret_cast<short*>(buffer);
}


/// <summary>
/// 5 байт из массива в 8 байт тип long long
/// </summary>
long long fiveBytesToLong(Byte* buffer)
{
	long long value = 0;
	memcpy(&value, buffer, 5);
	return value;
}


/// <summary>
/// 6 байт из массива в 8 байт тип long long
/// </summary>
long long sixBytesToLong(Byte* buffer)
{
	long long value = 0;
	memcpy(&value, buffer, 6);
	return value;
}


/// <summary>
///  3 байт из массива в 4 байт тип int
/// </summary>
int threeBytesToInt(Byte* buffer)
{
	int value = 0;
	memcpy(&value, buffer, 3);
	return value;
}


/// <summary>
/// из массива взять size байт и поместить в строку
/// </summary>
string buffToString(Byte* buffer, int size)
{
	return string((char*)(buffer), size);
}


void PrintBuffer(Byte *buffer, int size)
{
    cout<< endl <<"Размер: "<< size << endl;
    for (int i =0; i < size; i++)
    {
        printf("%02X ", buffer[i]);
    }
}


/// <summary>
/// из массива взять 4 байта и вернуть время в строке
/// </summary>
string buffToTimeStampStr(Byte *buffer)
{
    std::uint32_t time_date_stamp = buffToInteger(buffer);
    time_t temp = time_date_stamp;
    tm* t = gmtime(&temp);
    stringstream ss;
    ss << put_time(t, "%Y-%m-%d %I:%M:%S %p");
    return ss.str();
}


//Удалить пробелы и непечатуемые символы из строки
void removeSpecialChars(string& str)
{

    str.erase(std::remove_if(str.begin(), str.end(),
                                 [](char c)
                                 {
                                     return std::isspace(c) || c=='\n' || c=='\t' || c=='\r';
                                 }),
              str.end());
}


