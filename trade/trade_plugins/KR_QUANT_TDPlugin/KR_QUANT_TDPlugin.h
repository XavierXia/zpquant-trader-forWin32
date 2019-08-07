#ifndef QFCOMPRETRADESYSTEM_ATMTRADEPLUGINS_KR_QUANT_TDPLUGIN_TEMPLATE_ANY_TDPlugin_H_
#define QFCOMPRETRADESYSTEM_ATMTRADEPLUGINS_KR_QUANT_TDPLUGIN_TEMPLATE_ANY_TDPlugin_H_
#include <boost/thread.hpp>
#include <thread>                // std::thread
#include <mutex>                // std::mutex, std::unique_lock
#include <condition_variable>    // std::condition_variable
#include <atomic>
#include <boost/asio.hpp>
#include <memory>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <thread>
#include <future>
#include <tuple>
#include <boost/log/common.hpp>
#include "AtmTradePluginInterface.h"
#include "TradePluginContextInterface.h"

#include "oes_client_api.h"
#include "oes_client_spi.h"
#include "nn.hpp"
#include <nanomsg/pair.h>

#include "SeverityLevel.h"

#include <boost/property_tree/ptree.hpp>  
#include <boost/property_tree/json_parser.hpp> 
using namespace boost::property_tree;

using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace boost::asio;
using namespace std;

class CKR_QUANT_TDPlugin :
	public MAtmTradePluginInterface
{


	boost::log::sources::severity_logger< severity_levels > m_Logger;



	date GetTradeday(ptime _Current);
	date m_dateTradeDay;
	boost::shared_mutex m_mtxProtectCancelAmount;
	map<string, int> m_mapCancelAmount;
	int m_intInitAmountOfCancelChancesPerDay;

	Quant360::OesClientApi  *pOesApi;
	Quant360::OesClientSpi  *pOesSpi;

	string m_strUsername;//Init at TDInit
	string m_strPassword;//Init at TDInit

public:
	static const string s_strAccountKeyword;
	CKR_QUANT_TDPlugin();
	~CKR_QUANT_TDPlugin();
	int m_intRefCount = 0;
	atomic_bool m_abIsPending;
	bool IsPedding();
	nn::socket tdnnsocket;

	virtual bool IsOnline();
	virtual void IncreaseRefCount();
	virtual void DescreaseRefCount();
	virtual int GetRefCount();
	virtual void CheckSymbolValidity(const unordered_map<string, string> &);
	virtual string GetCurrentKeyword();
	virtual string GetProspectiveKeyword(const ptree &);
	virtual void GetState(ptree & out);
	virtual void TDInit(const ptree &, MTradePluginContextInterface*, unsigned int AccountNumber);
	virtual void TDHotUpdate(const ptree &);
	virtual void TDUnload();


	virtual TOrderRefIdType TDBasicMakeOrder(
		TOrderType ordertype,
		unordered_map<string, string> & instrument,
		TOrderDirectionType direction,
		TOrderOffsetType offset,
		TVolumeType volume,
		TPriceType LimitPrice,
		TOrderRefIdType orderRefBase
		);
	
	virtual TLastErrorIdType TDBasicCancelOrder(TOrderRefIdType, unordered_map<string, string> &, TOrderSysIdType);
	virtual int TDGetRemainAmountOfCancelChances(const char *);

	//交易相关
	int32 OesClientMain_SendOrder(Quant360::OesClientApi *pOesApi,
        uint8 mktId, const char *pSecurityId, const char *pInvAcctId,
        uint8 ordType, uint8 bsType, int32 ordQty, int32 ordPrice);
	int32 OesClientMain_CancelOrder(Quant360::OesClientApi *pOesApi,
        uint8 mktId, const char *pSecurityId, const char *pInvAcctId,
        int32 origClSeqNo, int8 origClEnvId, int64 origClOrdId);
	int32 OesClientMain_QueryClientOverview(Quant360::OesClientApi *pOesApi);
	int32 OesClientMain_QueryMarketStatus(Quant360::OesClientApi *pOesApi,
        uint8 exchId, uint8 platformId);
	int32 OesClientMain_QueryCashAsset(Quant360::OesClientApi *pOesApi,
        const char *pCashAcctId);
	int32 OesClientMain_QueryStock(Quant360::OesClientApi *pOesApi,
        const char *pSecurityId, uint8 mktId, uint8 securityType,
        uint8 subSecurityType);
	int32 OesClientMain_QueryStkHolding(Quant360::OesClientApi *pOesApi,
        uint8 mktId, const char *pSecurityId);

private:
	bool Start();
	bool Stop();
	void ShowMessage(severity_levels, const char * fmt, ...);
	void TimerHandler(boost::asio::deadline_timer* timer, const boost::system::error_code& err);
private:
    static void *       tdThreadMain(void *pParams);

};
#endif

