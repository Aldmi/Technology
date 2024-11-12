#ifndef GIDURALCLIENT_NPKPVALIDATOR_H
#define GIDURALCLIENT_NPKPVALIDATOR_H

#include <math.h>

enum NpkpValidatorStatus {
    Ok,
    Skip,
    WaitingCorrectPacket
};

class NpkpValidator {

private:
    unsigned int NPKP = 4294967295;
    NpkpValidatorStatus npkpValidatorStatus;
    int WaitingCorrectPacketInvokeCounterMax = 5; //За 5 пакетов не пришло валидное NPKP + 1
    int WaitingCorrectPacketInvokeCounter = 0;

public:
    NpkpValidatorStatus Check_NPKP(unsigned int packetSequenceNumber)
    {
        //Если номер принятого пакета на 1 больше НПКП (с учетом перехода через 4294967295),
        //то такой пакет признается корректным, обрабатывается Клиентом, НПКП устанавливается равным номеру этого пакета, Клиент переходит к ожиданию следующего пакета
        if (packetSequenceNumber == NPKP + 1)
        {
            NPKP = packetSequenceNumber;
            npkpValidatorStatus= Ok;
            return npkpValidatorStatus;
        }
        //Если номер принятого пакета меньше или равен НПКП (с учетом перехода через 4294967295), то такой пакет признается некорректным и должен быть проигнорирован Клиентом, НПКП не изменяется,
        //Клиент переходит к ожиданию следующего пакета;
        else
        if ((packetSequenceNumber - NPKP) <= 3) //packetSequenceNumber <= NPKP
        {
            npkpValidatorStatus= Skip;
            return npkpValidatorStatus;
        }
        //Если разность между номером последнего принятого пакета и НПКП более 1 (с учетом перехода через 4294967295),
        //то такой пакет признается некорректным, игнорируется Клиентом, НПКП не изменяется. Клиент фиксирует факт потери одного (или нескольких) пакетов,
        //устанавливает номер первого потерянного пакета равным НПКП (с учетом перехода через 4294967295) и далее действует так как описано в разделе «Действия при потере пакетов».
        else
        {
//            if((WaitingCorrectPacketInvokeCounter++ % WaitingCorrectPacketInvokeCounterMax) == 0) {
//                pFunctionRetransmissionRequest(NPKP);
//            }
//            npkpValidatorStatus= WaitingCorrectPacket;
//            return WaitingCorrectPacket;

             //DEBUG--------
            cout <<"NPKP error" << "NPKP =" << NPKP <<" packetSequenceNumber =" << packetSequenceNumber << endl;
            NPKP = packetSequenceNumber;
            npkpValidatorStatus= Ok;
            return Ok;    //Отправляем пакет принудительно
             //DEBUG-------
        }
    }


    NpkpValidatorStatus GetStatus(){
        return npkpValidatorStatus;
    }

    //callback - запрос на повторную передачу потерянных пакетов
    int (*pFunctionRetransmissionRequest)(int sequenceNumberFirstLostPacket){nullptr};
};


#endif //GIDURALCLIENT_NPKPVALIDATOR_H
