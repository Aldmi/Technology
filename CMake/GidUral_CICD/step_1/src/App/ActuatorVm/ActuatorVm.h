#ifndef GIDURALCLIENT_ACTUATORVM_H
#define GIDURALCLIENT_ACTUATORVM_H

#include "../../Shared/Shared.h"
#include "../Actuator/Actuator.h"

#include <memory>

class ActuatorVm {

public:
    int esr_id;
    string datatype;
    unsigned int packetSequenceNumber; //Порядковый номер пакета
    string timeCodRep;                 //Время создания файла cod_rep в формате Unix, приведенное к GMT
    string idObjGid;                   //3-символьный идентификатор объекта ГИД (в кодировке Windows-1251) (например: «83J»)
    Byte packetType;                   //Тип пакета
    Actuator actuator;                 //ИУ

    ActuatorVm(
            Actuator actuator,
            unsigned int packetSequenceNumber,
            string timeCodRep,
            string idObjGid,
            Byte packetType)
    {
        this->actuator = actuator;
        this->esr_id= actuator.ESR_RP;
        this->datatype="ui_info";

        this->packetSequenceNumber = packetSequenceNumber;
        this->timeCodRep = timeCodRep;
        this->idObjGid = idObjGid;
        this->packetType = packetType;
    }


    string SerializeToJson() const
    {
        std::stringstream ss;
        cereal::JSONOutputArchive archive(ss);
        archive(
                CEREAL_NVP(esr_id),
                CEREAL_NVP(datatype),
                CEREAL_NVP(packetSequenceNumber),
                CEREAL_NVP(timeCodRep),
                CEREAL_NVP(idObjGid),
                CEREAL_NVP(packetType),
                cereal::make_nvp("payload", actuator)
                );
        auto jsonStr=ss.str() + "\n}";
        return jsonStr;
    }
};

#endif //GIDURALCLIENT_ACTUATORVM_H
