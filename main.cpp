#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include "MainServer.h"

#define SERVER_DEBUG

#ifdef SERVER_DEBUG

#define KAFKA_HOSTS "192.168.100.70:9092,192.168.100.71:9092,192.168.100.72:9092"
#define FRONT_ADDR "tcp://218.202.237.33:10002"

#else

#define KAFKA_HOSTS "116.62.157.24:9092,120.27.239.140:9092,47.99.117.182:9092"
#define FRONT_ADDR "tcp://218.202.237.33:10002"

#endif

int main(){
#ifndef SERVER_DEBUG
    daemon(0, 0);
#endif
    CLogger *logger = nullptr;
    logger = new CLogger("SettServer", "log.properties");
    while(true){
        pid_t pid = fork();
        if (pid > 0){
            wait(NULL);
            logger->error("账单获取服务异常退出，主服务重启账单获取服务\n");
            sleep(1);
        }else if (pid == 0) {
            MainServer *server = nullptr;
            server = new MainServer(KAFKA_HOSTS, FRONT_ADDR, logger);
            server->init();
            server->run();
            server->destory();
            exit(0);
        }else{
            logger->error("创建子进程失败\n");
            return -1;
        }
    }
}


//int main01() {
//    CLogger *logger = nullptr;
//    logger = new CLogger("SettServer", "log.properties");
//
//    MainServer *server = nullptr;
//    server = new MainServer(KAFKA_HOSTS, FRONT_ADDR, logger);
//#ifndef SERVER_DEBUG
//    daemon(0, 0);
//#endif
//
//    int ret = server->init();
//    if (ret != 0){
//        delete(server);
//        delete(logger);
//        exit(-1);
//    }
//
//    while(!ret){
//        try{
//            server->run();
//            server->destory();
//            ret = server->init();
//        }catch (...){
//            logger->error("主服务器出现异常退出，1秒后重新启动");
//            server->destory();
//            sleep(1);
//            ret = server->init();
//        }
//    }
//    server->destory();
//    delete(server);
//    delete(logger);
//    return 0;
//}