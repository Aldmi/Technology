#ifndef GIDURALCLIENT_GIDURALPROTOCOL_H
#define GIDURALCLIENT_GIDURALPROTOCOL_H

#include "../../Shared/Shared.h"
#include "../Actuator/Actuator.h"
#include "../../Settings/Settings.h"
#include "../ActuatorVm/ActuatorVm.h"
#include <vector>
#include <stdexcept>

class GidUralProtocolAbstract {
private:
    static const int offsetHeaderEnd = 14;                          //14байт заголовка
    static const int offsetToArrayActuators = offsetHeaderEnd + 10; //14байт заголовка + 10 байт данных.
    inline static uint32_t lastTimeStampUt=0;                       //Последнее сохраненное значение времени (для 0x11 пакета)

public:
    static vector<ActuatorVm>* CreateActuatorVm_Type_13(Byte* buffer, size_t bufSize)
    {
        auto packetSize = buffToInteger(buffer + 2);
        auto packetSequenceNumber = buffToUnsignedInteger(buffer + 6);
        auto idObjGid =  buffToString(buffer + 10, 3);
        auto packetType = buffer[13];

        auto subArr= buffer + offsetHeaderEnd + 4; //14байт заголовка. 4байт - мусор
        auto actuator = Actuator(subArr);
        auto filter_passed= actuator.equalEsr(Settings::esr_expected);

        if(filter_passed) {
            auto vm= ActuatorVm(actuator, packetSequenceNumber, "", idObjGid, packetType);
            auto pVector = new vector<ActuatorVm>();
            pVector->emplace_back(vm);
            return pVector;
        }

        return nullptr;
    }


    static vector<ActuatorVm>* CreateActuatorVm_Type_11(Byte* buffer, size_t bufSize)
    {
        auto pVector = new vector<ActuatorVm>;
        try
        {
            auto packetSize = buffToInteger(buffer + 2);
            auto packetSequenceNumber = buffToUnsignedInteger(buffer + 6);
            auto idObjGid =  buffToString(buffer + 10, 3);
            auto packetType = buffer[13];

            auto subArr= buffer + offsetHeaderEnd;              //начало секции с даными
            std::uint32_t time_stamp = buffToInteger(subArr + 4);
            if(time_stamp == lastTimeStampUt) {
                cout << "Skip buffer, because lastTimeStampUt already exist" << lastTimeStampUt << endl;
                return nullptr;
            }
            time_stamp = lastTimeStampUt;
            auto lengthDataStruct = buffToInteger(subArr);
            auto timeCodRep = buffToTimeStampStr(subArr + 4);
            auto actuatorsCount = buffToShort(subArr + 8);

            subArr= buffer + offsetToArrayActuators;                   //начало секции с массивом ИУ
            for (int i = 0; i < actuatorsCount; i++)
            {
                auto actuator = Actuator(subArr);
                auto filter_passed= actuator.equalEsr(Settings::esr_expected);
                if(filter_passed) {
                    auto vm= ActuatorVm(actuator, packetSequenceNumber, timeCodRep, idObjGid, packetType);
                    pVector->emplace_back(vm);
                }
                auto offset= actuator.getOffset();
                if(offset == -1){
                    throw std::runtime_error("Error: offset invalid");
                }
                subArr += offset;
            }
            return pVector;
        }
        catch (std::runtime_error error)
        {
            delete pVector;
            pVector= nullptr;
            std::cout <<"CreateActuatorVm_Type_11.runtime_error"<< error.what() << std::endl;
            return nullptr;
        }
        catch (...)
        {
            delete pVector;
            pVector= nullptr;
            std::cout << "CreateActuatorVm_Type_11.Unexpected exception" << std::endl;
            return nullptr;
        }
    }
};

#endif //GIDURALCLIENT_GIDURALPROTOCOL_H
