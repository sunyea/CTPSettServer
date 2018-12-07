//
// Created by liaop on 18-6-27.
//

#include <unistd.h>
#include <list>
#include <deque>
#include "CKfkConsumer.h"
#include "../json/cJSON.h"
#include "../handler/CCustomCtpTradeSpi.h"

bool _run = true;

CKfkConsumer::CKfkConsumer(const string &brokers, const string &topics, string &groupid, int64_t offset,
                           string &front_addr, CLogger *logger) {
    _brokers = brokers;
    _topics = topics;
    _groupid = groupid;
    _offset = offset;
    strcpy(_front_addr, front_addr.c_str());
    _logger = logger;
}

bool CKfkConsumer::init(){
    char c_err[128] = {0};
    //******* Kafka Consumer实例 ********
    RdKafka::Conf* _conf = nullptr;
    _conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
    if (!_conf) {
        _logger->error("创建全局配置失败");
        return false;
    }

    std::string errstr;
    if (_conf->set("bootstrap.servers", _brokers, errstr) != RdKafka::Conf::CONF_OK) {
        memset(c_err, 0, sizeof(c_err));
        sprintf(c_err, "设置服务器列表失败，原因：%s", errstr.c_str());
        _logger->error(c_err);
    }

    if (_conf->set("group.id", _groupid, errstr) != RdKafka::Conf::CONF_OK) {
        memset(c_err, 0, sizeof(c_err));
        sprintf(c_err, "设置群组失败，原因：%s", errstr.c_str());
        _logger->error(c_err);
    }

    if (_conf->set("enable.auto.offset.store", "true", errstr) != RdKafka::Conf::CONF_OK) {
        memset(c_err, 0, sizeof(c_err));
        sprintf(c_err, "设置自动保存偏移量失败，原因：%s", errstr.c_str());
        _logger->error(c_err);
    }

    std::string str_fetch_num = "10240000";
    if (_conf->set("max.partition.fetch.bytes", str_fetch_num, errstr) != RdKafka::Conf::CONF_OK) {
        memset(c_err, 0, sizeof(c_err));
        sprintf(c_err, "设置单个分区拉取消息的最大尺寸失败，原因：%s", errstr.c_str());
        _logger->error(c_err);
    }

    _consumer = RdKafka::Consumer::create(_conf, errstr);
    if (!_consumer) {
        _logger->error("创建消费者实例失败！");
    }
    delete _conf;

    //******* Kafka Topic *******
    RdKafka::Conf *_topic_conf = nullptr;
    _topic_conf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);
    if (!_topic_conf) {
        memset(c_err, 0, sizeof(c_err));
        sprintf(c_err, "创建主题配置失败！");
        _logger->error(c_err);
        return false;
    }
//    if (_topic_conf->set("offset.store.path", KAFKA_OFFSET_FILE, errstr) != RdKafka::Conf::CONF_OK) {
//        fprintf(stderr, "设置属性offset保存路径失败，原因：%s\n", errstr.c_str());
//    }
//    if (_topic_conf->set("offset.store.method", "file", errstr) != RdKafka::Conf::CONF_OK) {
//        fprintf(stderr, "设置属性offset保存方法失败，原因：%s\n", errstr.c_str());
//    }
//    if (_topic_conf->set("offset.store.sync.interval.ms", "1", errstr) != RdKafka::Conf::CONF_OK) {
//        fprintf(stderr, "设置属性offset保存间隔时间失败，原因：%s\n", errstr.c_str());
//    }
    if (_topic_conf->set("auto.offset.reset", "latest", errstr) != RdKafka::Conf::CONF_OK) {
        memset(c_err, 0, sizeof(c_err));
        sprintf(c_err, "设置属性offset.reset失败，原因：%s", errstr.c_str());
        _logger->error(c_err);
    }
    _topic = RdKafka::Topic::create(_consumer, _topics, _topic_conf, errstr);
    if (!_topic) {
        memset(c_err, 0, sizeof(c_err));
        sprintf(c_err, "创建主题失败，原因：%s", errstr.c_str());
        _logger->error(c_err);
    }
    delete _topic_conf;

    //启动消费者
    RdKafka::ErrorCode resp = _consumer->start(_topic, _partition, _offset);
    if (resp != RdKafka::ERR_NO_ERROR) {
        memset(c_err, 0, sizeof(c_err));
        sprintf(c_err, "启动消费者失败，原因：%s", RdKafka::err2str(resp).c_str());
        _logger->error(c_err);
    }
    _logger->info("创建Kafka Consumer成功");

    return true;

}

//获取数据
bool CKfkConsumer::getData(int timeout_ms) {
    RdKafka::Message *msg = nullptr;

    while(_run) {
        msg = _consumer->consume(_topic, _partition, timeout_ms);
        consumer(msg);
        _consumer->poll(0);
        delete msg;
    }

    _consumer->stop(_topic, _partition);
    if(_topic) {
        delete _topic;
        _topic = nullptr;
    }
    if(_consumer) {
        delete _consumer;
        _consumer = nullptr;
    }

    RdKafka::wait_destroyed(5000);
    return true;
}
//处理获取的消息
void CKfkConsumer::consumer(RdKafka::Message *msg) {
    char c_err[128] = {0};
    switch (msg->err()) {
        case RdKafka::ERR__TIMED_OUT:
            break;
        //*********正确获取信息后进行信息处理***********
        case RdKafka::ERR_NO_ERROR:
//            fprintf(stdout, "%d %s\n",
//                   static_cast<int>(msg->len()),
//                   static_cast<char*>(msg->payload()));
            _logger->debug(static_cast<char*>(msg->payload()));
            last_offset = msg->offset();

            cJSON *get_root, *action, *sessionid, *data;
            get_root = cJSON_Parse(static_cast<char*>(msg->payload()));
            action = cJSON_GetObjectItem(get_root, "action");
            sessionid = cJSON_GetObjectItem(get_root, "sessionid");
            data = cJSON_GetObjectItem(get_root, "data");
            if(action and sessionid and data){
                cJSON *data_root, *uid, *brokerid, *inverstorid, *investopwd, *tradingday;
                if(data->valuestring == NULL){
                    data_root = data;
                }else{
                    data_root = cJSON_Parse(data->valuestring);
                }
                uid = cJSON_GetObjectItem(data_root, "uid");
                brokerid = cJSON_GetObjectItem(data_root, "brokerid");
                inverstorid = cJSON_GetObjectItem(data_root, "inverstorid");
                investopwd = cJSON_GetObjectItem(data_root, "investopwd");
                tradingday = cJSON_GetObjectItem(data_root, "tradingday");
                int tradingday_len = cJSON_GetArraySize(tradingday);
                deque<string> tradingdays;
                for (int i = 0; i < tradingday_len; i++){
                    cJSON *item = cJSON_GetArrayItem(tradingday, i);
                    string value = item->valuestring;
                    tradingdays.push_back(value);
                }
                string s_brokerid = brokerid->valuestring;
                string s_inverstorid = inverstorid->valuestring;
                string s_investopwd = investopwd->valuestring;
                string s_sessionid = sessionid->valuestring;
                string s_uid = "4";
                if(uid){
                    s_uid = uid->valuestring;
                }
//                fprintf(stderr, "brokerid:%s, id:%s, pwd:%s, uid:%s, sessionid:%s\n", s_brokerid.c_str(),
//                        s_inverstorid.c_str(), s_investopwd.c_str(), s_uid.c_str(), s_sessionid.c_str());
                getSett(s_brokerid, s_inverstorid, s_investopwd, tradingdays, s_sessionid, s_uid);
            }
            break;
        //******************************************
        case RdKafka::ERR__PARTITION_EOF:
            memset(c_err, 0, sizeof(c_err));
            sprintf(c_err, "达到队列末尾，偏移值：%d", (int)last_offset);
            _logger->debug(c_err);
            break;
        case RdKafka::ERR__UNKNOWN_TOPIC:
        case RdKafka::ERR__UNKNOWN_PARTITION:
            memset(c_err, 0, sizeof(c_err));
            sprintf(c_err, "消费失败，原因：%s", msg->errstr().c_str());
            _logger->debug(c_err);
            _run = false;
            break;
        default:
            memset(c_err, 0, sizeof(c_err));
            sprintf(c_err, "消费失败，原因:%s", msg->errstr().c_str());
            _logger->debug(c_err);
            _run = false;
            break;
    }
}

void CKfkConsumer::destroy() {
    if(_topic) {
        delete _topic;
        _topic = nullptr;
    }
    if(_consumer) {
        delete _consumer;
        _consumer = nullptr;
    }
}

void CKfkConsumer::getSett(string &brokerID, string &investorID, string &investorPWD, deque<string> &tradingDay,
                           string &sessionid, string &uid) {
    CThostFtdcTraderApi *traderApi = CThostFtdcTraderApi::CreateFtdcTraderApi();
    CCustomCtpTradeSpi *pTraderSpi = new CCustomCtpTradeSpi();
    pTraderSpi->Init(traderApi, brokerID, investorID, investorPWD, tradingDay, sessionid, uid, _logger);
    traderApi->RegisterSpi(pTraderSpi);
    traderApi->SubscribePublicTopic(THOST_TERT_RESTART);
    traderApi->SubscribePrivateTopic(THOST_TERT_RESTART);
    traderApi->RegisterFront(_front_addr);
    traderApi->Init();
    while(true){
        if(pTraderSpi->settFinished == true){
            _logger->info("本用户获取账单结束。");
            break;
        }
        usleep(100000);
    }
    delete(pTraderSpi);
    traderApi->Release();
}
