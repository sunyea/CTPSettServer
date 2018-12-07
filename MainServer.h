//
// Created by liaop on 18-11-30.
//

#ifndef CTPSETTSERVER_MAINSERVER_H
#define CTPSETTSERVER_MAINSERVER_H

#include <iostream>
#include <mutex>
#include <queue>
#include "logger/CLogger.h"
#include "kafka/CKfkConsumer.h"
#include "kafka/CKfkProducer.h"

#define TOPIC_IN "tp.js.sett"
#define TOPIC_OUT "tp.js"
#define GROUP_ID "gp.js.sett"

using namespace std;

class MainServer {
public:
    MainServer(const string brokers, const string front_addr, CLogger *logger);

    int init();
    void destory();
    void run();

private:
    string _brokers;
    string _front_addr;

    CLogger *_logger;
};


#endif //CTPSETTSERVER_MAINSERVER_H
