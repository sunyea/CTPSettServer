//
// Created by liaop on 18-6-29.
//

#include <cstring>
#include "CKfkProducer.h"

CKfkProducer::CKfkProducer(string &brokers, string &topics, int partition, CLogger *logger) {
    _brokers = brokers;
    _topics = topics;
    _partition = partition;
    _logger = logger;
}

bool CKfkProducer::Init() {
    char c_err[128] = {0};
    string errstr;
    //创建Producer实例，并进行设置
    RdKafka::Conf *_conf = nullptr;
    _conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    if(!_conf) {
        memset(c_err, 0, sizeof(c_err));
        sprintf(c_err, "创建Producer配置失败");
        _logger->error(c_err);
        return false;
    }
    if(_conf->set("bootstrap.servers", _brokers, errstr) != RdKafka::Conf::CONF_OK) {
        memset(c_err, 0, sizeof(c_err));
        sprintf(c_err, "设置Brokers失败：%s", errstr.c_str());
        _logger->error(c_err);
    }
    if(_conf->set("batch.num.messages", "1000", errstr) != RdKafka::Conf::CONF_OK) {
        memset(c_err, 0, sizeof(c_err));
        sprintf(c_err, "设置batch.num.messages失败：%s", errstr.c_str());
        _logger->error(c_err);
    }
    if(_conf->set("queue.buffering.max.ms", "500", errstr) != RdKafka::Conf::CONF_OK) {
        memset(c_err, 0, sizeof(c_err));
        sprintf(c_err, "设置queue.buffering.max.ms失败：%s", errstr.c_str());
        _logger->error(c_err);
    }
    _producer = RdKafka::Producer::create(_conf, errstr);
    if(!_producer) {
        memset(c_err, 0, sizeof(c_err));
        sprintf(c_err, "创建Producer实例失败：%s", errstr.c_str());
        _logger->error(c_err);
    }
    delete _conf;

    //创建Topic实例，并配置
    RdKafka::Conf *_topic_conf = nullptr;
    _topic_conf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);
    if(!_topic_conf) {
        memset(c_err, 0, sizeof(c_err));
        sprintf(c_err, "创建Topic配置失败");
        _logger->error(c_err);
        return false;
    }
    _topic = RdKafka::Topic::create(_producer, _topics, _topic_conf, errstr);
    if(!_topic) {
        memset(c_err, 0, sizeof(c_err));
        sprintf(c_err, "创建Topic实例失败：%s", errstr.c_str());
        _logger->error(c_err);
    }
    delete _topic_conf;
    _logger->info("创建Kafka Producer成功");



    return true;
}

CKfkProducer::~CKfkProducer() {
    if(_topic) {
        delete _topic;
    }
    if(_producer) {
        delete _producer;
    }
}

bool CKfkProducer::PushData(void *data, size_t data_size) {
    if(data) {
        RdKafka::ErrorCode resp = _producer->produce(_topic, RdKafka::Topic::PARTITION_UA, RdKafka::Producer::RK_MSG_COPY,
                                                     data, data_size, NULL, NULL);
        if(resp != RdKafka::ERR_NO_ERROR) {
            char c_err[128] = {0};
            sprintf(c_err, "推送信息失败：%s", RdKafka::err2str(resp).c_str());
            _logger->error(c_err);
            return false;
        }
        _producer->poll(0);
    }
    return true;
}