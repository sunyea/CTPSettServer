//
// Created by liaop on 18-11-30.
//


#include <thread>
#include <cstring>
#include "MainServer.h"
#include "handler/CMultiThread.h"


MainServer::MainServer(const string brokers, const string front_addr, CLogger *logger) {
    _brokers = brokers;
    _front_addr = front_addr;
    _logger = logger;
}

int MainServer::init() {
    _logger->info("准备启动账单获取服务...");
    return 0;
}

void MainServer::destory() {
    _logger->info("账单获取服务停止。");
}

void MainServer::run() {
    CMultiThread myThread;
    myThread.Init(_brokers, _front_addr, _logger);
    thread th1(CMultiThread::getFromKafka, TOPIC_IN, GROUP_ID, &myThread);
    thread th2(CMultiThread::putToKafka, TOPIC_OUT, &myThread);
    th1.join();
    th2.join();
}
