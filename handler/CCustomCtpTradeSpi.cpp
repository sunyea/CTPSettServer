//
// Created by liaop on 18-7-18.
//

#include <cstdio>
#include <thread>
#include "CCustomCtpTradeSpi.h"
#include "Cgbk.cpp"
#include "CMultiThread.h"

//初始化
void CCustomCtpTradeSpi::Init(CThostFtdcTraderApi* traderApi, string &brokerID, string &investorID, string &investorPWD,
                              deque<string> &tradingDay, string &sessionid, string &uid, CLogger *logger) {
    _traderApi = traderApi;
    _brokerID = brokerID;
    _investorID = investorID;
    _investorPWD = investorPWD;
    _tradingDay = tradingDay;
    _sessionid = sessionid;
    _uid = uid;
    _logger = logger;
    loginFlag = false;
    settFinished = false;
}

//当建立通讯连接时
void CCustomCtpTradeSpi::OnFrontConnected() {
    _logger->info("CTP建立网络链接成功。");
    //开始登录
    reqUserLogin();
}

// 登录应答
void CCustomCtpTradeSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo,
                                        int nRequestID, bool bIsLast) {
    if (!isErrorRspInfo(pRspInfo)) {
        _logger->info("CTP账户登录成功。");
        loginFlag = true;
        //保存会话参数
        _trade_font_id = pRspUserLogin->FrontID;
        _session_id = pRspUserLogin->SessionID;
        strcpy(_order_ref, pRspUserLogin->MaxOrderRef);

        //请求投资者结算结果
        reqSettlement();
    }
}

//错误应答
void CCustomCtpTradeSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    _logger->debug("OnRspError");
    isErrorRspInfo(pRspInfo);
    settFinished = true;
}

//网络断开时
void CCustomCtpTradeSpi::OnFrontDisconnected(int nReason) {
    memset(_error, 0, sizeof(_error));
    sprintf(_error, "网络断开, 错误码：%d", nReason);
    _logger->error(_error);
    settFinished = true;
}

//心跳超时
void CCustomCtpTradeSpi::OnHeartBeatWarning(int nTimeLapse) {
    memset(_error, 0, sizeof(_error));
    sprintf(_error, "心跳超时，距离上次时间：%d毫秒", nTimeLapse);
    _logger->error(_error);
}

//账户登出应答
void CCustomCtpTradeSpi::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo,
                                         int nRequestID, bool bIsLast) {
    if (!isErrorRspInfo(pRspInfo)) {
        loginFlag = false;
        memset(_error, 0, sizeof(_error));
        sprintf(_error, "账户 %s 登出成功。", pUserLogout->UserID);
        _logger->error(_error);
    }
}

//用户结算结果应答
void CCustomCtpTradeSpi::OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo,
                                                CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
    if(pSettlementInfo) {
        strcat(_sett, pSettlementInfo->Content);
        if (bIsLast){
//            _logger->info(_sett);
            Sett sett;
            sett.sessionid = _sessionid;
            sett.uid = _uid;
            sett.sett = new char[102400];
            strcpy(sett.sett, _sett);
//            printf("sett_size:%d\n", strlen(_sett));
            CMultiThread::_mutex.lock();
            CMultiThread::_Queue.push_back(sett);
            CMultiThread::_mutex.unlock();
            memset(_sett, 0, sizeof(_sett));
            sleep(1);
            reqSettlement();
        }
    }else{
//        _logger->error("没有账单");
        sleep(1);
        memset(_sett, 0, sizeof(_sett));
        reqSettlement();
    }

}

//判断是否收到错误信息
bool CCustomCtpTradeSpi::isErrorRspInfo(CThostFtdcRspInfoField *pRspInfo) {
    bool bResult = pRspInfo && (pRspInfo->ErrorID != 0);
    if (bResult){
        memset(_error, 0, sizeof(_error));
        sprintf(_error, "返回错误，错误编号：%d，错误信息：%s", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
        _logger->error(_error);
    }
    return bResult;
}

//用户登录请求
void CCustomCtpTradeSpi::reqUserLogin() {
    CThostFtdcReqUserLoginField loginReq;
    memset(&loginReq, 0, sizeof(loginReq));
    strcpy(loginReq.BrokerID, _brokerID.c_str());
    strcpy(loginReq.UserID, _investorID.c_str());
    strcpy(loginReq.Password, _investorPWD.c_str());
    static int requestID = 0;
    int rt = _traderApi->ReqUserLogin(&loginReq, ++requestID);
    if (!rt) {
//        _logger->info("已发送登录请求");
    }else{
        _logger->error("发送登录请求失败");
    }
}

//用户登出请求
void CCustomCtpTradeSpi::reqUserLogout() {
    CThostFtdcUserLogoutField logoutReq;
    memset(&logoutReq, 0, sizeof(logoutReq));
    strcpy(logoutReq.BrokerID, _brokerID.c_str());
    strcpy(logoutReq.UserID, _investorID.c_str());
    static int requestID = 0;
    int rt = _traderApi->ReqUserLogout(&logoutReq, ++requestID);
    if (!rt) {
        _logger->info("已发送登出请求");
    }else{
        _logger->error("发送登出请求失败");
    }
}

//请求结算结果
void CCustomCtpTradeSpi::reqSettlement() {
    CThostFtdcQrySettlementInfoField settReq;
    static int requestID = 0;
    if (!_tradingDay.empty()) {
        string item = _tradingDay.back();
        memset(&settReq, 0, sizeof(CThostFtdcQrySettlementInfoField));
        strcpy(settReq.BrokerID, _brokerID.c_str());
        strcpy(settReq.TradingDay, item.c_str());
        int rt = _traderApi->ReqQrySettlementInfo(&settReq, ++requestID);
        if (!rt) {
//            _logger->info("已发送结算结果请求");
            _tradingDay.pop_back();
        } else {
            _logger->error("发送结算结果请求失败");
        }
    }else{
        _logger->debug("该用户账单获取完毕");
        settFinished = true;
    }
}

