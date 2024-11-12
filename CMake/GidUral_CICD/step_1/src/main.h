#ifndef GIDURALCLIENT_MAIN_H
#define GIDURALCLIENT_MAIN_H

#include <iostream>

#include "Shared/Shared.h"
#include "App/GidUralProtocol/GidUralProtocol.h"
#include "App/Parser/Parser.h"
#include "TcpIpClient/TcpIpClient.h"
#include "Tests/MoqData.h"
#include "Settings/Settings.h"
#include "Kafka/KafkaProducer/KafkaProducer.h"
#include "App/ActuatorVm/ActuatorVm.h"

#define VERSION "1.0.1"


//Запрос на восстановление пакетов
int RetransmissionRequest(int packetSequenceNumber);
//Отправить данные на kafka
void SendByKafka(vector<ActuatorVm>* pVectorVm);

//Обработчик данных от TCP socket
int RawDataHandler(Byte buffer[], size_t size);
int RawDataConsumer_Invoker(Byte buffer[], size_t size);

#endif //GIDURALCLIENT_MAIN_H
