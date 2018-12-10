//
// Created by liaop on 18-7-18.
//

#ifndef MDPRODUCER_CCUSTOMCTPTRADESPI_H
#define MDPRODUCER_CCUSTOMCTPTRADESPI_H

#include <iostream>
#include <string>
#include <cstring>
#include <list>
#include <deque>
#include <queue>
#include "../ctpapi/ThostFtdcTraderApi.h"
#include "../logger/CLogger.h"
#include "../handler/CMultiThread.h"

using namespace std;

typedef struct {
    char *sett;
    int size;
} settString;

class CCustomCtpTradeSpi: public CThostFtdcTraderSpi {
public:
    ///初始化
    void Init(CThostFtdcTraderApi* traderApi, string &brokerID, string &investorID, string &investorPWD,
              deque<string> &tradingDay, string &sessionid, string &uid, CLogger *logger);

    ///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
    void OnFrontConnected();

    ///登录请求响应
    void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///错误应答
    void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
    void OnFrontDisconnected(int nReason);

    ///心跳超时警告。当长时间未收到报文时，该方法被调用。
    void OnHeartBeatWarning(int nTimeLapse);

    ///登出请求响应
    void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    ///投资者结算结果确认响应
    void OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

public:
    bool loginFlag;     // 登陆成功的标识
    bool settFinished;  // 结算单完毕
    settString _psett;

private:
    string _brokerID;       //经纪商代码
    string _investorID;   //投资者账户
    string _investorPWD;    //投资者密码
    deque<string> _tradingDay;     //交易日
    string _sessionid;
    string _uid;

    CLogger *_logger = nullptr;
    char _error[64] = {0};
//    char _sett[102400] = {0};

    //会话参数
    TThostFtdcFrontIDType _trade_font_id;   //前置编号
    TThostFtdcSessionIDType _session_id;    //会话编号
    TThostFtdcOrderRefType _order_ref;      //报单引用
    time_t _orderTime;
    time_t _orderOkTime;

    //api指针
    CThostFtdcTraderApi* _traderApi = nullptr;
private:

    void reqUserLogin(); // 登录请求

    void reqUserLogout(); // 登出请求

    void reqSettlement();   //请求投资者结果

    bool isErrorRspInfo(CThostFtdcRspInfoField *pRspInfo); // 是否收到错误信息
};


#endif //MDPRODUCER_CCUSTOMCTPTRADESPI_H
