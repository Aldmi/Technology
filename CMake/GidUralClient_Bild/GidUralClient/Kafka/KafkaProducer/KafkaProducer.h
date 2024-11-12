#ifndef KAFKA_TEST_KAFKAPRODUCER_H
#define KAFKA_TEST_KAFKAPRODUCER_H

#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <cstring>

#include <unistd.h>
#include <thread>

#if _AIX
#include <unistd.h>
#endif
#include <librdkafka/rdkafkacpp.h>

using namespace std;


static volatile sig_atomic_t run = 1;
static void sigterm(int sig) {
    run = 0;
}


class ExampleDeliveryReportCb : public RdKafka::DeliveryReportCb {
public:
    void dr_cb(RdKafka::Message &message) {
        if (message.err())
            std::cerr << "% Message delivery failed: " << message.errstr()
                      << std::endl;
        else
            std::cout << "% Message delivered to topic " << message.topic_name()
                      << " [" << message.partition() << "] at offset "
                      << message.offset() << std::endl;
    }
};

class KafkaProducer {

private:
    RdKafka::Producer *_producerP;
    string _topic;

    KafkaProducer(RdKafka::Producer *producerP, string topic){
        _producerP = producerP;
        _topic = topic;
    }

public:
    ~KafkaProducer(){
        _producerP->flush(10 * 1000 /* wait for max 10 seconds */);
        if (_producerP->outq_len() > 0)
            std::cerr << "% " << _producerP->outq_len()<< " message(s) were not delivered" << std::endl;
        delete _producerP;
    }

    static KafkaProducer* CreteProducer(string brokers, string topic, ExampleDeliveryReportCb &deliveryReportCb, std::string &errstr){

        RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
        if (conf->set("bootstrap.servers", brokers, errstr) != RdKafka::Conf::CONF_OK) {
            return nullptr;
        }

        signal(SIGINT, sigterm);
        signal(SIGTERM, sigterm);

        if (conf->set("dr_cb", &deliveryReportCb, errstr) != RdKafka::Conf::CONF_OK) {
            return nullptr;
        }

        RdKafka::Producer *producer = RdKafka::Producer::create(conf, errstr);
        if (!producer) {
            std::cerr << "Failed to create producer: " << errstr << std::endl;
            return nullptr;
        }

        delete conf;

        auto instance= new KafkaProducer(producer, topic);
        return instance;
    }


    void SendMessage(string message)
    {
        RdKafka::Headers *headers = RdKafka::Headers::create();
        headers->add("my header", "header value");
        headers->add("other header", "yes");
        //асинхронный вызов
        retry:
        RdKafka::ErrorCode err = _producerP->produce(
                /* Topic name */
                _topic,
                /* Any Partition: the builtin partitioner will be
                 * used to assign the message to a topic based
                 * on the message key, or random partition if
                 * the key is not set. */
                RdKafka::Topic::PARTITION_UA,
                /* Make a copy of the value */
                RdKafka::Producer::RK_MSG_COPY /* Copy payload */,
                /* Value */
                const_cast<char *>(message.c_str()), message.size(),
                /* Key */
                NULL, 0,
                /* Timestamp (defaults to current time) */
                0,
                /* Message headers, if any */
                headers,
                /* Per-message opaque value passed to
                 * delivery report */
                NULL);

        if (err != RdKafka::ERR_NO_ERROR) {
            std::cerr << "% Failed to produce to topic " << _topic << ": "
                      << RdKafka::err2str(err) << std::endl;

            if (err == RdKafka::ERR__QUEUE_FULL) {
                /* If the internal queue is full, wait for
                 * messages to be delivered and then retry.
                 * The internal queue represents both
                 * messages to be sent and messages that have
                 * been sent or failed, awaiting their
                 * delivery report callback to be called.
                 *
                 * The internal queue is limited by the
                 * configuration property
                 * queue.buffering.max.messages and queue.buffering.max.kbytes */
                _producerP->poll(1000 /*block for max 1000ms*/);
                goto retry;
            }

        } else {
            std::cout << "% Enqueued message (" << message.size() << " bytes) "
                      << "for topic " << _topic << std::endl;
        }

        _producerP->poll(0);
    }
};

#endif //KAFKA_TEST_KAFKAPRODUCER_H
