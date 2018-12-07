//
// Created by liaop on 18-12-5.
//

#ifndef CTPSETTSERVER_CMULTITHREAD_H
#define CTPSETTSERVER_CMULTITHREAD_H

#include <iostream>
#include <mutex>
#include <list>
#include "../logger/CLogger.h"

using namespace std;

typedef struct {
    string sessionid;
    string uid;
    char *sett;
} Sett;

class CMultiThread {
public:
    CMultiThread();
    void Init(string hosts, string front_addr, CLogger *logger);

    static void getFromKafka(string topic, string group_id, void* mThread);
    static void putToKafka(string topic, void* mThread);

    static list<Sett> _Queue;
    static mutex _mutex;

private:


    string _hosts;
    string _front_addr;
    CLogger *_logger;
};


#endif //CTPSETTSERVER_CMULTITHREAD_H
