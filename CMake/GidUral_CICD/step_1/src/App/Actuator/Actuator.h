#ifndef GIDURALCLIENT_ACTUATOR_H
#define GIDURALCLIENT_ACTUATOR_H

#include "../../Shared/Shared.h"

enum ActuatorType
{
    Undefined,
    Full,  //50байт
    Small  //31байт
};


/// <summary>
/// Запись о состоянии ИУ (полная и сокращенная)
/// </summary>
struct Actuator
{
    Actuator()	{  }

    Actuator(Byte* arr)
    {
        if(arr[0] == 50) actuatorType = Full;
        else if(arr[0] == 31)  actuatorType = Small;
        else actuatorType = Undefined;

        if(actuatorType != Undefined)
        {
            ESR_RP = buffToInteger(arr + 4);
            ESR_PARK = buffToInteger(arr + 8);
            Name = buffToString(arr + 12, 10);
            Code_DC_DK = buffToShort(arr + 22);
            Address = fiveBytesToLong(arr + 24);
            if (actuatorType == Full)
            {
                actuatorTypeStr = "Big";
                NumberOfTrain= buffToShort(arr + 29);
                ColorTrainThread = buffToInteger(arr + 31);
                SpecialFeaturesTrain = arr[35];
                ConditionalLengthTrain = arr[36];
                TrainGrossWeight = buffToShort(arr + 37);
                TrainIndexInASOUP = sixBytesToLong(arr + 39);
                HeadLocomotiveSeries = buffToShort(arr + 45);
                HeadLocomotiveNumber = threeBytesToInt(arr + 47);
                ActuatorTypeStatus = 2;
            }
            else
            {
                actuatorTypeStr = "Small";
                auto status = buffToShort(arr + 29);
                ActuatorTypeStatus = (status == 0) ? 0 : 1;
            }
        }
        else{
            ErrorMessage = "type IU is not defined !!!";
        }
    }

    ActuatorType actuatorType;             //1..4 Размер структуры данных (50 - полная ИУ  31 - сокращенная ИУ)
    string actuatorTypeStr;                //actuatorType в стрковом виде
    int ESR_RP;                            //5..8 ЕСР-код РП, на котором находится ИУ. 5-значный код ЕСР станции (например: «58322»);
    int ESR_PARK;                          //9..12 ЕСР-код (условный) парка, на котором находится ИУ
    string Name;                           //13..22 Символьное обозначение ИУ (в кодировке Windows-1251)
    unsigned short Code_DC_DK;             //23..24 Условный код системы ДЦ / ДК, в которой работает ИУ
    long long Address;                     //25..29 Адрес ИУ (5байт)

    unsigned short NumberOfTrain;          //30..31 Номер поезда на ИУ
    int ColorTrainThread;                  //32..35 Цвет нитки поезда
    Byte SpecialFeaturesTrain;             //36 Спец-признаки поезда
    Byte ConditionalLengthTrain;           //37 Условная длина поезда
    unsigned short TrainGrossWeight;       //38..39 Вес поезда брутто (с тарой)
    long long TrainIndexInASOUP;           //40..45 Индекс поезда в АСОУП  (6байт)
    unsigned short HeadLocomotiveSeries;   //46..47 Серия головного локомотива (2байта)
    int HeadLocomotiveNumber;              //48..50 Номер головного локомотива (3байта)

    int ActuatorTypeStatus;                 //если actuatorType==Small. 30..31 Номер поезда на ИУ 0 – если ИУ свободен,FFFFh – если поезд не идентифицирован

    string ErrorMessage;                   //Сообщение ошибки


    int getOffset() const {
        if(actuatorType == Full) return 50;
        else if(actuatorType == Small) return 31;
        else return -1;
    }

    bool equalEsr(const std::vector<int>& esr_expected) {
        if(esr_expected.empty()){
            return true;
        }
       auto esr= std::find(esr_expected.begin(), esr_expected.end(), ESR_RP);
       return esr != esr_expected.end();
    }

    // This method lets cereal know which data members to serialize
    template<class Archive>
    void serialize(Archive & archive)
    {
        switch (actuatorType) {
            case Full:
                archive(
                        cereal::make_nvp("type", actuatorTypeStr),
                        cereal::make_nvp("esr_rp", ESR_RP),
                        cereal::make_nvp("esr_park", ESR_PARK),
                        cereal::make_nvp("iu_name", Name),
                        cereal::make_nvp("dc_system_code", Code_DC_DK),
                        cereal::make_nvp("iu_address", Address),
                        cereal::make_nvp("iu_train_number", NumberOfTrain),
                        cereal::make_nvp("iu_train_color", ColorTrainThread),
                        cereal::make_nvp("iu_train_attributes", SpecialFeaturesTrain),
                        cereal::make_nvp("iu_train_length", ConditionalLengthTrain),
                        cereal::make_nvp("iu_train_wegth", TrainGrossWeight),
                        cereal::make_nvp("iu_train_index_asoup", TrainIndexInASOUP),
                        cereal::make_nvp("iu_loco_ser", HeadLocomotiveSeries),
                        cereal::make_nvp("iu_loco_num", HeadLocomotiveNumber),
                        cereal::make_nvp("iu_status", ActuatorTypeStatus)
                );
                break;

            case Small:
                archive(
                        cereal::make_nvp("type", actuatorTypeStr),
                        cereal::make_nvp("esr_rp", ESR_RP),
                        cereal::make_nvp("esr_park", ESR_PARK),
                        cereal::make_nvp("iu_name", Name),
                        cereal::make_nvp("dc_system_code", Code_DC_DK),
                        cereal::make_nvp("iu_address", Address),
                        cereal::make_nvp("iu_status", ActuatorTypeStatus)
                );
                break;

            case Undefined:
                archive(
                        CEREAL_NVP(ErrorMessage)
                );
                break;
        }
    }
};

#endif //GIDURALCLIENT_ACTUATOR_H
