//
// Created by liaop on 18-6-29.
//

#ifndef MDPRODUCER_CKFKPRODUCER_H
#define MDPRODUCER_CKFKPRODUCER_H

#include <iostream>
#include <string>
#include "librdkafka/rdkafkacpp.h"
#include "../logger/CLogger.h"

using namespace std;

class CKfkProducer {
public:
    CKfkProducer(string &brokers, string &topics, int partition, CLogger *logger);
    ~CKfkProducer();

    bool Init();
    bool PushData(void *data, size_t data_size);

private:
    string _brokers;
    string _topics;
    int _partition;
    CLogger *_logger;

    RdKafka::Producer *_producer = nullptr;
    RdKafka::Topic *_topic = nullptr;

};


#endif //MDPRODUCER_CKFKPRODUCER_H
