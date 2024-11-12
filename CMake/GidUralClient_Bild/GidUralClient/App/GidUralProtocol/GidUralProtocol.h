#ifndef GIDURALCLIENT_GIDURALPROTOCOL_H
#define GIDURALCLIENT_GIDURALPROTOCOL_H


#include "../../Shared/Shared.h"
#include "../Actuator/Actuator.h"
#include "../../Settings/Settings.h"
#include <vector>

class GidUralProtocolAbstract {

protected:
    long PacketSize;                //Размер пакета
    long PacketSequenceNumber;      //Порядковый номер пакета
    string IdObjGid;                //3-символьный идентификатор объекта ГИД (в кодировке Windows-1251) (например: «83J»)
    Byte PacketType;                //Тип пакета
    string ErrorMessage;            //Сообщение об ошибке

public:
    bool filter_passed;    //флаг, прошел ли фильтр
    static const int offsetHeaderEnd = 14;//14байт заголовка


    GidUralProtocolAbstract(string errorMessage) {
        ErrorMessage = errorMessage;
    }

    GidUralProtocolAbstract(Byte* arr, size_t arrSize) {
        PacketSize = buffToInteger(arr + 2);
        PacketSequenceNumber = buffToInteger(arr + 6);
        IdObjGid =  buffToString(arr + 10, 3);
        PacketType = arr[13];
    }

    virtual string SerializeToJson() const {return {};}
};


class CurrentStateAllActuators : public GidUralProtocolAbstract {

private:
    int lengthDataStruct;         //Размер структуры данных в пакете до массива записей о состояниях ИУ
    string timeCodRep;            //Время создания файла cod_rep в формате Unix, приведенное к GMT
    short actuatorsCount;         //Количество ИУ в пакете
    vector<Actuator> actuators;   //Массив записей о состоянии ИУ

public:
    static const int offsetToArrayActuators = offsetHeaderEnd + 10;//14байт заголовка + 10 байт данных.

    CurrentStateAllActuators(Byte* arr, size_t arrSize)
            : GidUralProtocolAbstract(arr, arrSize)
    {
        auto subArr= arr + offsetHeaderEnd;//начало секции с даными
        lengthDataStruct = buffToInteger(subArr);
        timeCodRep = buffToTimeStampStr(subArr + 4);
        actuatorsCount = buffToShort(subArr + 8);

        subArr= arr + offsetToArrayActuators;//начало секции с массивом ИУ
        for (int i = 0; i < actuatorsCount; i++)
        {
            auto actuator = Actuator(subArr);
            if(actuator.equalEsr(Settings::esr_expected)) {
                actuators.emplace_back(actuator);
            }
            auto offset= actuator.getOffset();
            if(offset == -1){
                break;
            }
            subArr +=actuator.getOffset();
        }
        filter_passed= true;
    }

    string SerializeToJson() const override // явным образом указываем, что функция переопределена
    {
        std::stringstream ss;
        cereal::JSONOutputArchive archive(ss); // Create an output archive
        archive(cereal::make_nvp("ALL", *this));
        auto jsonStr=ss.str() + "\n}";
        return jsonStr;
    }


    template<class Archive>
    void serialize(Archive & archive)
    {
        if(ErrorMessage.empty())
        {
            archive(
                    CEREAL_NVP(PacketSize),
                    CEREAL_NVP(PacketSequenceNumber),
                    CEREAL_NVP(IdObjGid),
                    CEREAL_NVP(PacketType),
                    CEREAL_NVP(timeCodRep),
                    CEREAL_NVP(actuatorsCount),
                    CEREAL_NVP(actuators)
            );
        }
        else
        {
            archive(
                    CEREAL_NVP(ErrorMessage)
            );
        }
    }
};



class CurrentStateSingleActuator : public GidUralProtocolAbstract {

private:
    Actuator actuator;     //состояние ИУ

public:

    CurrentStateSingleActuator(Byte* arr, size_t arrSize)
            : GidUralProtocolAbstract(arr, arrSize)
    {
        auto subArr= arr + offsetHeaderEnd + 4; //14байт заголовка. 4байт - мусор
        actuator = Actuator(subArr);
        filter_passed= actuator.equalEsr(Settings::esr_expected);
    }

    string SerializeToJson() const override
    {
        std::stringstream ss;
        cereal::JSONOutputArchive archive(ss);
        archive(cereal::make_nvp("SINGLE", *this));
        auto jsonStr= ss.str() + "\n}";
        return jsonStr;                                                          //todo: Возвращать jsonStr по ссылке. (избежать копирования)
    }


    template<class Archive>
    void serialize(Archive & archive)
    {
        if(ErrorMessage.empty())
        {
            archive(
                    CEREAL_NVP(PacketSize),
                    CEREAL_NVP(PacketSequenceNumber),
                    CEREAL_NVP(IdObjGid),
                    CEREAL_NVP(PacketType),
                    CEREAL_NVP(actuator)
            );
        }
        else
        {
            archive(
                    CEREAL_NVP(ErrorMessage)
            );
        }
    }
};

#endif //GIDURALCLIENT_GIDURALPROTOCOL_H
