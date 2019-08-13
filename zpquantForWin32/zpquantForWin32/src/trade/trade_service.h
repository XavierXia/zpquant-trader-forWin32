#ifndef _QFCOMPRETRADESYSTEM_ATM_TRADESERVICE_H_
#define _QFCOMPRETRADESYSTEM_ATM_TRADESERVICE_H_

#include "public.h"

#ifdef KR_QUANT_MDPlugin
#include "KR_QUANT_MDPlugin/KR_QUANT_MDPlugin.h"
#endif

#ifdef KR_QUANT_TDPlugin
#include "KR_QUANT_TDPlugin/KR_QUANT_TDPlugin.h"
#endif

#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <sstream>
#include <queue>
#include <unordered_map>
#include <boost/thread/mutex.hpp>
#include <boost/thread/shared_mutex.hpp>
#include "CommuModForServInterface.h"
#include "AtmMarketDataPluginInterface.h"
#include "AtmTradePluginInterface.h"
#include "TradePluginContextInterface.h"
#include "SeverityLevel.h"
#include <iostream>

using namespace std;
using namespace std::placeholders;
#define MEDDLE_RESPONSE_MAXLEN 1024
#define STRATEGY_MESSAGE_MAXLEN 1024

typedef std::shared_ptr<MAtmPluginInterface> PluginPtrType;
typedef std::shared_ptr<MAtmMarketDataPluginInterface> MDPluginPtrType;
typedef std::shared_ptr<MAtmTradePluginInterface> TDPluginPtrType;
typedef PluginPtrType(*TPluginFactory)();

#define PLUGIN(name,classname) {name,make_pair([]()->PluginPtrType {return PluginPtrType(new classname());},classname::s_strAccountKeyword)}
enum class PackageHandlerParamType
{
    MarketData,
    Trade,
    Nothing
};
class CTradeService :
    public CCommuModForServSpi,
	public MTradePluginContextInterface
{
    const unordered_map<string, pair<TPluginFactory, string> > m_mapAMarketDataPFactories =
    {
#ifdef KR_QUANT_MDPlugin
        PLUGIN("kr_md_quant",CKrQuantMDPluginImp)
#endif
    };

    const unordered_map<string, pair<TPluginFactory, string> > m_mapATradePFactories =
    {
#ifdef KR_QUANT_TDPlugin
        PLUGIN("kr_td_quant",CKR_QUANT_TDPlugin)
#endif
    };
    typedef void (CTradeService::*TPackageHandlerFuncType)(PackageHandlerParamType, const ptree & in, ptree &out);
#define HANDLER(fn,param) (std::bind(&CTradeService::fn,this,PackageHandlerParamType::param,std::placeholders::_1,std::placeholders::_2))


    const unordered_map<string, function<void(const ptree & in, ptree &out)>> m_mapString2PackageHandlerType = {
        //增加源
        { "reqaddmarketdatasource",        HANDLER(ReqAddSource,                MarketData) },
        { "reqaddtradesource",            HANDLER(ReqAddSource,                Trade) },
    };

    std::string m_strConfigFile;
    unsigned int m_uSystemNumber = 0;

    //管理所有行情源与交易源，这两个数据结构需要互斥保护
    pair<vector<PluginPtrType>, boost::shared_mutex> m_vecAllMarketDataSource;//行情源数组
    pair<vector<PluginPtrType>, boost::shared_mutex> m_vecAllTradeSource;//交易源数组

    boost::shared_mutex m_mtxSharedValue;

public:
    CTradeService(std::string configFile,unsigned int sysnum);
    ~CTradeService();
    void Start();
    void Join();
private:
	void MakeError(ptree & out, const char * fmt, ...);
    string GetAddress();
    unsigned short GetListenPort();
    size_t GetNetHandlerThreadCount();
    
    MCommuModForServInterface * m_pApi = nullptr;
    virtual void OnCommunicate(const ptree & in, ptree & out);

#define PACKAGE_HANDLER(fun_name) void fun_name (PackageHandlerParamType,const ptree & in, ptree &out);
    PACKAGE_HANDLER(ReqGetSupportedTypes)
    PACKAGE_HANDLER(ReqGetAllSource)
    PACKAGE_HANDLER(ReqAddSource)
    PACKAGE_HANDLER(ReqDelSource)
    PACKAGE_HANDLER(ReqAllStrategyBin)
    PACKAGE_HANDLER(ReqAllArchiveFile)
    PACKAGE_HANDLER(ReqDeployNewStrategy)
    PACKAGE_HANDLER(ReqGetAllRunningStrategies)
    PACKAGE_HANDLER(ReqCancelRunningStrategies)
    PACKAGE_HANDLER(ReqGetProbe)
    PACKAGE_HANDLER(ReqMeddle)
    PACKAGE_HANDLER(ReqGetMeddleResponse)
    PACKAGE_HANDLER(ReqStrategyParams)
    PACKAGE_HANDLER(ReqStrategyConfigJson)
    PACKAGE_HANDLER(ReqUpdateStrategyBin)
    PACKAGE_HANDLER(ReqModifySharedValue)
    PACKAGE_HANDLER(ReqAllSharedValue)
    PACKAGE_HANDLER(ReqSetOrderTickets)
    PACKAGE_HANDLER(ReqGetPositionInfo)
    PACKAGE_HANDLER(ReqGetCustomInfo)
    PACKAGE_HANDLER(ReqGetFloatingProfit)
    PACKAGE_HANDLER(ReqStatus)

};
#endif
