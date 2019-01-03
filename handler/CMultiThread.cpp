//
// Created by liaop on 18-12-5.
//

#include <unistd.h>
#include <cstring>
#include "CMultiThread.h"
#include "../kafka/CKfkProducer.h"
#include "../kafka/CKfkConsumer.h"
#include "../json/cJSON.h"

CMultiThread::CMultiThread() {
    //
}

void CMultiThread::Init(string hosts, string front_addr, CLogger *logger) {
    _hosts = hosts;
    _front_addr = front_addr;
    _logger = logger;
}

void CMultiThread::getFromKafka(string topic, string group_id, void *mThread) {
    CMultiThread* thisThread = (CMultiThread*)mThread;
    CKfkConsumer *consumer = new CKfkConsumer(thisThread->_hosts, topic, group_id, -1, thisThread->_front_addr, thisThread->_logger);

    if(consumer){
        thisThread->_logger->info("启动消费者服务线程..");
        consumer->init();
        consumer->getData(300);
    }else{
        thisThread->_logger->info("消费者线程启动失败");
    }
}

void CMultiThread::putToKafka(string topic, void *mThread) {
    CMultiThread* thisThread = (CMultiThread*)mThread;
    CKfkProducer *producer = new CKfkProducer(thisThread->_hosts, topic, 0, thisThread->_logger);


    if (producer){
        producer->Init();
        thisThread->_logger->info("启动生产者线程..");
        while(1){
            if(CMultiThread::_Queue.empty()){
                sleep(1);
            }else{
                CMultiThread::_mutex.lock();
                Sett item = CMultiThread::_Queue.front();
                CMultiThread::_Queue.pop_front();
                CMultiThread::_mutex.unlock();
                cJSON *_root, *_data;
                _root = cJSON_CreateObject();
                cJSON_AddStringToObject(_root, "sessionid", item.sessionid.c_str());
                cJSON_AddStringToObject(_root, "action", "save_statement");
                _data = cJSON_CreateObject();
                cJSON_AddStringToObject(_data, "uid", item.uid.c_str());
                cJSON_AddStringToObject(_data, "date", "");
                cJSON_AddStringToObject(_data, "text", item.sett);
                cJSON_AddItemToObject(_root, "data", _data);
                string s_send = cJSON_Print(_root);
                cJSON_Delete(_root);
                producer->PushData((void *)s_send.c_str(), s_send.length());
//                modify by lp @ 2018-12-29
                if(item.sett != nullptr) {
                    free(item.sett);
                    item.sett = nullptr;
                }
//                modify end.
            }
        }
    }else{
        thisThread->_logger->info("生产者线程启动失败");
    }
}

mutex CMultiThread::_mutex;
list<Sett> CMultiThread::_Queue;
