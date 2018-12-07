//
// Created by liaop on 18-11-30.
//

#ifndef CTPSETTSERVER_CLOGGER_H
#define CTPSETTSERVER_CLOGGER_H

#include <iostream>
#include <log4cplus/logger.h>

using namespace std;


class CLogger {
public:
    /**
     * 构建函数
     * @param name Logger名称
     * @param profile 配置文件地址
     */
    CLogger(const string name, const string profile);
    ~CLogger();

    /**
     * 写日志，根据不同函数生成不同级别的日志
     * @param msg 日志内容
     */
    void trace(const string msg);
    void debug(const string msg);
    void info(const string msg);
    void warn(const string msg);
    void error(const string msg);
    void fatal(const string msg);

private:
    log4cplus::Logger _logger;
};


#endif //CTPSETTSERVER_CLOGGER_H
