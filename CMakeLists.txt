cmake_minimum_required(VERSION 3.4)
project(CTPSettServer)

set(CMAKE_CXX_STANDARD 11)

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -march=native -O3 -pthread" )

add_executable(CTPSettServer main.cpp handler/Cgbk.cpp ctpapi/ThostFtdcTraderApi.h ctpapi/ThostFtdcUserApiDataType.h ctpapi/ThostFtdcUserApiStruct.h kafka/CKfkConsumer.h kafka/CKfkConsumer.cpp kafka/CKfkProducer.h kafka/CKfkProducer.cpp handler/CCustomCtpTradeSpi.h handler/CCustomCtpTradeSpi.cpp logger/CLogger.cpp logger/CLogger.h MainServer.cpp MainServer.h handler/CMultiThread.cpp handler/CMultiThread.h json/cJSON.h json/cJSON.c)

target_link_libraries(CTPSettServer /usr/local/lib/librdkafka++.so ${PROJECT_SOURCE_DIR}/ctpapi/libthosttraderapi.so /usr/local/lib/liblog4cplus.so)