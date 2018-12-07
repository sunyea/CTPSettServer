//
// Created by liaop on 18-6-27.
//

#ifndef CTPMDSERVICE_CKFKCONSUMER_H
#define CTPMDSERVICE_CKFKCONSUMER_H

#include <csignal>
#include <iostream>
#include <memory>
#include <string>
#include <queue>
#include <mutex>
#include "librdkafka/rdkafkacpp.h"
#include "../logger/CLogger.h"

#define KAFKA_OFFSET_FILE "kafka_offset.txt"

using namespace std;

class CKfkConsumer {
public:
    CKfkConsumer(const string &brokers, const string &topics, string &groupid, int64_t offset, string &front_addr, CLogger *logger);
    ~CKfkConsumer(){};

    bool init();
    bool getData(int timeout_ms);
    void destroy();

private:
    void consumer(RdKafka::Message *msg);
    void getSett(string &brokerID, string &investorID, string &investorPWD, deque<string> &tradingDay, string &sessionid, string &uid);
    string _brokers;
    string _topics;
    string _groupid;
    char _front_addr[128]={0};
    CLogger *_logger;

    int64_t last_offset = 0;
    RdKafka::Consumer* _consumer = nullptr;
    RdKafka::Topic* _topic = nullptr;
    int64_t _offset = RdKafka::Topic::OFFSET_BEGINNING;
    int32_t _partition = 0;


};


#endif //CTPMDSERVICE_CKFKCONSUMER_H
