//
// Created by liaop on 18-11-30.
//
#include <log4cplus/configurator.h>
#include <log4cplus/loggingmacros.h>
#include "CLogger.h"

using namespace std;
using namespace log4cplus;
using namespace log4cplus::helpers;

CLogger::CLogger(const string name, const string profile) {
    this->_logger = Logger::getInstance(LOG4CPLUS_TEXT(name));
    Logger root = Logger::getRoot();
    PropertyConfigurator::doConfigure(LOG4CPLUS_TEXT(profile));
}

CLogger::~CLogger() {
    this->_logger.shutdown();
}

void CLogger::trace(const string msg) {
    LOG4CPLUS_TRACE(this->_logger, LOG4CPLUS_TEXT(msg));
}

void CLogger::debug(const string msg) {
    LOG4CPLUS_DEBUG(this->_logger, LOG4CPLUS_TEXT(msg));
}

void CLogger::info(const string msg) {
    LOG4CPLUS_INFO(this->_logger, LOG4CPLUS_TEXT(msg));
}

void CLogger::warn(const string msg) {
    LOG4CPLUS_WARN(this->_logger, LOG4CPLUS_TEXT(msg));
}

void CLogger::error(const string msg) {
    LOG4CPLUS_ERROR(this->_logger, LOG4CPLUS_TEXT(msg));
}

void CLogger::fatal(const string msg) {
    LOG4CPLUS_FATAL(this->_logger, LOG4CPLUS_TEXT(msg));
}